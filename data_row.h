#ifndef datastream_data_row_h
#define datastream_data_row_h

#include <map>
#include <list>
#include <sstream>

#include "data_element.h"
#include "schema_set.h"
#include "datastream_definitions.h"
#include "formatter.h"

namespace datastream {

	using std::map;
	using std::list;
	using std::ostream;

	class DataRow;

	typedef list<DataRow> DataRows;
	typedef vector<DataRow*> DataRowPointers;
	typedef map<int, DataRowPointers> DataRowPointersMap;

	class DataRow{
	public:

		unsigned int id;
		unsigned int parent;
		DataElements child_elements;
		DataRowPointersMap data_child_rows_by_set_id_map;

		//maybe look at using std reference wrapper to use refernces instead of pointers

		friend void writeGroup (
			ostream & os,
			RowWrapper parent_row_wrapper_type,
			SchemaSet& schema_set,
			const vector<DataRow*>* rows_ptr,  //so can be called with nullptr for empty group
			Formatter& formatter,
			unsigned int & siblings_written
		){
			if(
				rows_ptr == nullptr ||
				rows_ptr->size() < 1
			){
				if (schema_set.hide_when_empty == false){

					//DEV FIX!!
					// formatter.writeGroupWithValue(
					// 	os,
					// 	schema_set.group_name,
					// 	schema_set.row_name,
					// 	parent_row_wrapper_type,
					// 	schema_set.groupWrapper,
					// 	schema_set.rowWrapper
					//
					// 	//if group unwrapped : null
					// 	//if group wrapped: []
					//
					// 	//move these to end and make default
					// 		blank,
					// 		true,
					// 		ElementDatatype::type_raw,
					// );
				}

				return;
			}



			formatter.openGroup(
				os,
				schema_set.group_name,
				schema_set.row_name,
				siblings_written,

				//parent row wrapper
				parent_row_wrapper_type, //schema_set.rowWrapper,
				schema_set.limit_single_child,
				schema_set.groupWrapper
			);

			auto child_row_ptr_it = rows_ptr->begin();

			unsigned int children_written = 0;

			while (child_row_ptr_it != rows_ptr->end()){

				(*child_row_ptr_it)->write(os, schema_set, formatter, children_written);
				child_row_ptr_it++;

			}

			//if rows:

			formatter.closeGroup(
				os,
				schema_set.group_name,
				schema_set.row_name,
				siblings_written,

				//parent row wrapper
				parent_row_wrapper_type, //schema_set.rowWrapper,
				//schema_set.limit_single_child,
				schema_set.groupWrapper
			);

			++siblings_written;
		}




		DataRow (unsigned int id, unsigned int parent):
			id(id),
			parent(parent)
		{
		};

		void load(list<SchemaElement> & schema_elements, const string & line_values){

			auto schema_elements_it = schema_elements.begin();
			auto schema_elements_it_end = schema_elements.end();

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

				// this development version of the code interprets the string 'null'
				// to be a null value
				// the live version will not do this

				// add child element with no value
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

		void write(ostream & os, SchemaSet& schema_set, Formatter& formatter, unsigned int & siblings_written) const {

			formatter.openRow(
				os,
				schema_set.row_name,
				schema_set.rowWrapper,
				siblings_written
			);

			auto schema_child_element_it = schema_set.child_elements.begin();
			auto data_child_element_it = child_elements.begin();

			unsigned int children_written = 0;

			//WRITE CHILD ELEMENTS
			for(;
				schema_child_element_it != schema_set.child_elements.end() &&
				data_child_element_it != child_elements.end();

				schema_child_element_it++,
				data_child_element_it++

			){
				//what matters:

				// parent row format : does this need a label?

				formatter.writeElement(
					os,
					schema_child_element_it->name,
					data_child_element_it->getValue(),
					data_child_element_it->isNull(),
					schema_child_element_it->datatype,

					//parent row wrapper
					schema_set.rowWrapper,
					children_written
				);

				++children_written;
			}

			// the schema tells us which sets nest within the current set

			// we use the integer id from the child set to retrieve rows
			// from a map which is a member of this object

			// why such a convoluted path to the data we alreay have in this object?

			// 1.
			// because there may not be any child rows in the data for this row,
			// but we still may want to add an empty label

			// 2.
			// because this ensure that the sets are presented in the correct order
			// no the order which the map is sorted

			auto schema_child_set_it = schema_set.child_sets.begin();
			for (;
				schema_child_set_it < schema_set.child_sets.end();
				schema_child_set_it++
			){
				auto data_child_rows_it = data_child_rows_by_set_id_map.find((*schema_child_set_it)->id);

				const vector<DataRow*> * rows_ptr = nullptr;

				if(data_child_rows_it != data_child_rows_by_set_id_map.end()){
						rows_ptr = &data_child_rows_it->second;
				}

				writeGroup (
					os,	//stream
					schema_set.rowWrapper, //parent row wrapper
					**schema_child_set_it, //schema set for rows
					rows_ptr,  //so can be called with nullptr for empty group
					formatter,
					children_written //number of siblings written
				);
			}

			formatter.closeRow(
				os,
				schema_set.row_name,
				schema_set.rowWrapper
			);

			++siblings_written;
		}

		void nestChildRow(int set_id, DataRow* row_ptr){
			if(
				data_child_rows_by_set_id_map.find(set_id) ==
				data_child_rows_by_set_id_map.end()
			){
				data_child_rows_by_set_id_map.emplace(set_id, DataRowPointers());
			}
			data_child_rows_by_set_id_map[set_id].push_back(row_ptr);
		}
	};
}
#endif
