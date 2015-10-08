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

	class DataRow{
	public:

		unsigned int id;
		unsigned int parent;
		list<DataElement> child_elements;

		// this should not be a concrete member
		// but a reference, perhaps shared pointer
		// so it can be shared by rows with the same id

		map<int, vector<DataRow*>> data_child_rows_by_set_id_map;

		friend void writeGroup (
			ostream & os,
			RowWrapper parent_row_wrapper_type,
			SchemaSet& schema_set,
			const vector<DataRow*>* row_ptrs_ptr,
			Formatter& formatter,
			unsigned int & siblings_written
		){
			if(
				row_ptrs_ptr == nullptr ||
				row_ptrs_ptr->size() < 1
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
					// 		ElementDataType::type_raw,
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

			unsigned int children_written = 0;

			for (auto child_row_ptr : *row_ptrs_ptr){
				child_row_ptr->write(os, schema_set, formatter, children_written);
			}

			formatter.closeGroup(
				os,
				schema_set.group_name,
				schema_set.row_name,
				siblings_written,

				parent_row_wrapper_type,
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

		void write(ostream & os, SchemaSet& schema_set, Formatter& formatter, unsigned int & siblings_written) const
		{
			formatter.openRow(
				os,
				schema_set.row_name,
				schema_set.rowWrapper,
				siblings_written
			);

			unsigned int children_written = 0;

			auto schema_child_element_it = schema_set.child_elements.begin();
			auto data_child_element_it = child_elements.begin();

			//WRITE CHILD ELEMENTS
			//make template for both http://stackoverflow.com/questions/12552277
			for(;
				schema_child_element_it != schema_set.child_elements.end() &&
				data_child_element_it != child_elements.end();

				schema_child_element_it++,
				data_child_element_it++

			){
				formatter.writeElement(
					os,
					schema_child_element_it->name,
					data_child_element_it->getValue(),
					data_child_element_it->isNull(),
					schema_child_element_it->data_type,

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
			// because this ensures that the sets are presented in the correct order
			// not the order which the map is sorted

			for (auto schema_child_set_ptr : schema_set.child_sets){

				auto data_child_rows_it = data_child_rows_by_set_id_map.find(schema_child_set_ptr->id);

				const vector<DataRow*> * rows_ptr = nullptr;

				if(data_child_rows_it != data_child_rows_by_set_id_map.end()){
					rows_ptr = &data_child_rows_it->second;
				}

				writeGroup (
					os,
					schema_set.rowWrapper, //parent row wrapper
					*schema_child_set_ptr, //schema set for rows
					rows_ptr,              //can be called with nullptr for empty group
					formatter,
					children_written       //number of siblings written
				);
			}

			formatter.closeRow(
				os,
				schema_set.row_name,
				schema_set.rowWrapper
			);

			++siblings_written;
		}

		void nestChildRow(int set_id, DataRow& child_row){
			if(
				data_child_rows_by_set_id_map.find(set_id) ==
				data_child_rows_by_set_id_map.end()
			){
				data_child_rows_by_set_id_map.emplace(set_id, vector<DataRow*>{&child_row});
			}
			else{
				data_child_rows_by_set_id_map[set_id].push_back(&child_row);
			}
		}
	};
}
#endif
