#include "data_row.h"

namespace datastream {

	using std::map;
	using std::vector;
	using std::shared_ptr;
	using std::list;
	using std::ostream;

	DataRow::DataRow (unsigned int id, unsigned int parent, unsigned int position):
	id_(id),
	parent_(parent),
	position_(position)
	{};

	void DataRow::load(const list<SchemaElement> & schema_elements, const string & line_values)
	{
		pattern_iterator const regex_end;
		pattern_iterator regex_it(
			line_values.begin(),
			line_values.end(),
			comma_pattern,
			-1
		);

		auto schema_elements_it = schema_elements.begin();

		while(
			schema_elements_it != schema_elements.end() &&
			regex_it != regex_end
		){
			//DEV ONLY
			//db version will not do this
			//null value is explicit not a string compare
			if (*regex_it == null_keyword){
				child_elements_.emplace_back();
			}
			else{
				// add element using schema formatter
				child_elements_
					.emplace_back((*schema_elements_it)
					.read(*regex_it));
			}

			schema_elements_it++;
			regex_it++;
		}
	};

	void DataRow::connect(int set_id, std::shared_ptr<std::vector<DataRow*>> child_rows)
	{
		child_rows_by_set_id_.emplace(set_id, child_rows);
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
            child_elements_,
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

					//parent wrappers
					schema_set.groupWrapper(),
					schema_set.rowWrapper(),
					children_written
				);
				++children_written;
			}
		);

		//write child rows
		for (auto child_schema_set : schema_set.childSets()){

			auto child_rows_it =  child_rows_by_set_id_.find(child_schema_set->id());

			if(child_rows_it != child_rows_by_set_id_.end()){
				writeRows (
					os,
					schema_set.rowWrapper(), 		//parent row wrapper
					*child_schema_set, 		//schema set for rows
					*child_rows_it->second, //rows
					formatter,
					children_written       		//number of siblings written
				);
			} else{
				writeRows (
					os,
					schema_set.rowWrapper(), 		//parent row wrapper
					*child_schema_set, 		//schema set for rows
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
