#include "data_row.h"

namespace datastream {

	using std::map;
	using std::vector;
	using std::shared_ptr;
	using std::list;
	using std::ostream;

	DataRow::DataRow (unsigned int id, unsigned int parent, unsigned int order):
	id_(id),
	parent_(parent),
	order_(order)
	{};

	void DataRow::load(const list<SchemaElement> & schema_elements, const string & line_values){

		auto schema_elements_it = schema_elements.begin();

		pattern_iterator const regex_end;
		pattern_iterator regex_it(
			line_values.begin(),
			line_values.end(),
			comma_pattern,
			-1
		);

		while(
			schema_elements_it != schema_elements.end() &&
			regex_it != regex_end
		){
			//DEV ONLY
			//db version will not do this
			//null value is explicit not a string compare
			if (*regex_it == null_keyword){
				child_elements.emplace_back();
			}
			else{
				// add element using schema formatter
				child_elements
					.emplace_back((*schema_elements_it)
					.read(*regex_it));
			}

			schema_elements_it++;
			regex_it++;
		}
	};

	void DataRow::connect(int set_id, std::shared_ptr<std::vector<DataRow*>> child_rows_ptr){
		data_child_rows_ptr_by_set_id_map.emplace(set_id, child_rows_ptr);
	}

	void DataRow::write(ostream & os, const SchemaSet& schema_set, Formatter& formatter, unsigned int & siblings_written) const
	{
		formatter.openRow(
			os,
			schema_set.rowName(),
			schema_set.rowWrapper(),
			siblings_written
		);

		unsigned int children_written = 0;

		boost::range::for_each(
			schema_set.childElements(),
            child_elements,
			[&](
				const SchemaElement& schema_child,
				const DataElement& data_child
			){
				formatter.writeElement(
					os,
					schema_child.name(),
					data_child.getValue(),
					data_child.isNull(),
					schema_child.dataType(),

					//parent row wrapper
					schema_set.rowWrapper(),
					children_written
				);
				++children_written;
			}
		);

		//WRITE CHILD ROWS
		for (auto schema_child_set_ptr : schema_set.childSets()){

			auto child_rows_ptr_it =  data_child_rows_ptr_by_set_id_map.find(schema_child_set_ptr->id());

			if(child_rows_ptr_it != data_child_rows_ptr_by_set_id_map.end()){
				writeRows (
					os,
					schema_set.rowWrapper(), 		//parent row wrapper
					*schema_child_set_ptr, 		//schema set for rows
					*child_rows_ptr_it->second, //rows
					formatter,
					children_written       		//number of siblings written
				);
			} else{
				writeRows (
					os,
					schema_set.rowWrapper(), 		//parent row wrapper
					*schema_child_set_ptr, 		//schema set for rows
					vector<DataRow*>{},    		//empty rows
					formatter,
					children_written       		//number of siblings written
				);
			}
		}

		formatter.closeRow(
			os,
			schema_set.rowName(),
			schema_set.rowWrapper()
		);

		++siblings_written;
	}
}
