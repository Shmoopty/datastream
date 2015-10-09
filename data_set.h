#ifndef datastream_data_set_h
#define datastream_data_set_h

#include <list>
#include <map>
#include "datastream_definitions.h"
#include "data_row.h"
#include "schema_set.h"
#include "formatter.h"

namespace datastream {

	using std::map;
	using namespace datastream;

	class DataSet{
	public:

		// unsigned int id; // <- in schema
		// unsigned int parent; //<- in schema

		DataSet(SchemaSet* schema_set_ptr):
			schema_set_ptr(schema_set_ptr)
		{
		};

		void load(list<SchemaElement> & schema_elements, const string & line){

			std::smatch matched;
			std::regex_match(
				line,
				matched,
				data_row_line_pattern
			);

			if (!matched.length()){
				throw std::domain_error("data row format does not match expected pattern.");
			}

			rows.emplace_back(
				std::stoi(matched[match_index_data_row_id]),
				std::stoi(matched[match_index_data_parent]),
				rows.size()
			);

			//current data row is last added
			rows.rbegin()->load(
				schema_elements,
				matched[match_index_data_element_values]
			);
		}

		void write (ostream & os, Formatter& formatter) const
		{
			unsigned int siblings_written = 0;
			formatter.open(os, schema_set_ptr->groupWrapper);

			writeRows (
				os,
				RowWrapper::no_wrapper, //parent row wrapper - root has not parent
				*schema_set_ptr, //schema set for rows
				rows,    //rows
				formatter,
				siblings_written
			);

			formatter.close(os, schema_set_ptr->groupWrapper);

		}

		void connect(DataSet& parent_set)
		{

			for (auto& parent_id_child_rows : rows_by_parent_map){

				auto parent_rows_search = parent_set.id_to_rows_map.find(parent_id_child_rows.first);

				if(parent_rows_search != parent_set.id_to_rows_map.end()){

					auto parent_rows = parent_rows_search->second;

					for(DataRow* parent_row : parent_rows)
					{
						parent_row->connect(getId(), parent_id_child_rows.second);
					}
				}
			}
		}

		// rows are stored in a list to give us a stable address
		// now put pointers to rows into vector
		// for sorting and random access

		void mapRows()
		{
			//id_to_rows_map.clear();

			//push pointer to rows from list into vector
			vector<DataRow*> row_ptrs;

			for(DataRow& row : rows )
			{
				row_ptrs.push_back(&row);
			}

			if(schema_set_ptr->child_sets.size() > 0){

				//sort vector by row id
				std::sort(
					row_ptrs.begin(),
					row_ptrs.end(),
					[](const DataRow* a, const DataRow* b){
						if (a->id == b->id)
						{
							return a->order < b->order;
						}
						return a->id < b->id;
					}
				);

				// map of vectors caters for multiple appearances of same id!
				for (DataRow* row_ptr : row_ptrs){

					if (id_to_rows_map.find(row_ptr->id) == id_to_rows_map.end()){
						id_to_rows_map.emplace_hint(
							id_to_rows_map.end(),
							row_ptr->id,
							vector<DataRow*>{row_ptr});
					}
					else{
						id_to_rows_map[row_ptr->id].push_back(row_ptr);
					}
				}
			}

			if(!schema_set_ptr->isRoot()){
				//sort vector by row parent
				std::sort(
					row_ptrs.begin(),
					row_ptrs.end(),
					[](const DataRow* a, const DataRow* b){
						if (a->parent == b->parent)
						{
							return a->order < b->order;
						}
						return a->parent < b->parent;
					}
				);

				// map rows by their parent
				for (DataRow* row_ptr : row_ptrs){

					auto rows_by_parent_map_search = rows_by_parent_map.find(row_ptr->parent);

					if (rows_by_parent_map_search == rows_by_parent_map.end()){
						rows_by_parent_map.emplace_hint(
							rows_by_parent_map.end(),
							row_ptr->parent,
							std::shared_ptr<std::vector<DataRow*>>(new std::vector<DataRow*>{row_ptr})
						);
					} else {
						rows_by_parent_map_search->second.get()->push_back(row_ptr);
					}
				}
			}
		}

		bool isRoot() const {
			if(schema_set_ptr == nullptr){
				throw std::domain_error("data structure is invalid. schema details not found. root unknown");
			}
			return schema_set_ptr->isRoot();
		}

		int getParentId() const {
			if(schema_set_ptr == nullptr){
				throw std::domain_error("data structure is invalid. schema details not found. parent unknown");
			}
			return schema_set_ptr->parent;
		}

		int getId() const {
			if(schema_set_ptr == nullptr){
				throw std::domain_error("data structure is invalid. schema details not found. schema id unknown");
			}
			return schema_set_ptr->id;
		}

		private:
			SchemaSet* schema_set_ptr;
			list<DataRow> rows;
			map<int, vector<DataRow*>> id_to_rows_map;
			//std::map<unsigned int, std::shared_ptr<DataRow*>> rows_by_parent_map;

			std::map<int, std::shared_ptr<std::vector<DataRow*>>> rows_by_parent_map;


	};
}
#endif
