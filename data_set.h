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

	class DataSet;
	typedef list<DataSet> DataSets;
	typedef DataSet* DataSetPointer;
	typedef map<int, DataSetPointer> DataSetPointersMap;

	class DataSet{
	public:
		// not all will be needed keep for now
		// cull unused in subsequent iteration
		// not a big waste because number of sets is low
		//
		unsigned int sort;
		// unsigned int id; // <- in schema
		// unsigned int parent; //<- in schema
		SchemaSet* schema_set_ptr;

		DataRows rows;
		DataRowPointersMap id_to_rows_map;

		DataSet(
				unsigned int sort,
				//unsigned int id,
				//unsigned int parent,
				SchemaSet* schema_set_ptr
			):
		sort(sort),
		// id(id),
		// parent(parent),
		schema_set_ptr(schema_set_ptr)
		{
			//cout << sort << "\n";
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
				//rows.size(),
				std::stoi(matched[match_index_data_row_id]),
				std::stoi(matched[match_index_data_parent])
			);

			//current data row is last added
			rows.rbegin()
			->load(
				schema_elements,
				matched[match_index_data_element_values]
			);
		}

		void write (ostream & os, Formatter& formatter) const
		{
			unsigned int siblings_written = 0;
			formatter.open(os, schema_set_ptr->groupWrapper);

			for (
				auto row_it = rows.begin();
				row_it != rows.end();
				++row_it
			){
				row_it->write(os, *schema_set_ptr, formatter, siblings_written);
			}

			formatter.close(os, schema_set_ptr->groupWrapper);

		}

		void weave(DataSet& parent)
		{
			weaveToRows(parent.id_to_rows_map);
		}

		void weaveToRows(DataRowPointersMap& parent_id_to_parent_rows_map)
		{
			for(
				auto
				row_it = rows.begin();
				row_it != rows.end();
				++row_it
			) {

				// parent id may match multiple rows
				// ids are present in the data
				// not by this process
				// this process must handle cases where id is not unique

				auto parent_rows_search = parent_id_to_parent_rows_map.find(row_it->parent);
				if(parent_rows_search != parent_id_to_parent_rows_map.end()){

					auto parent_rows = parent_rows_search->second;


					for(
						auto
						parent_row_it = parent_rows.begin();
						parent_row_it != parent_rows.end();
						++parent_row_it
					){
						(*parent_row_it)->nestChildRow(getId(), &(*row_it));
					}
				}
			}
		}

		// rows are stored in a list to give us a stable address
		// now put pointers to rows into vector
		// for sorting and random access

		void mapRows()
		{
			id_to_rows_map.clear();

			if(schema_set_ptr->child_sets.size() == 0){
				return;
			}

			//push pointer to rows from list into vector
			DataRowPointers row_ptrs;
			for(
				auto row_it = rows.begin();
				row_it != rows.end();
				++row_it
			){
				row_ptrs.push_back(&(*row_it));
			}

			//sort vector by element row id
			std::sort(
				row_ptrs.begin(),
				row_ptrs.end(),
				[](const DataRow* a, const DataRow* b){return a->id < b->id;}
			);

			// map of vectors caters for multiple appearances of same id!
			for (
				auto
				row_ptrs_it = row_ptrs.begin();
				row_ptrs_it != row_ptrs.end();
				++row_ptrs_it
			){
				map_row((*row_ptrs_it)->id, *row_ptrs_it);
			}
		}

		void map_row(int row_id, DataRow* row_ptr){

			// optimisation
			// try emplace_hint on id_to_rows_map.rend()?

			if (id_to_rows_map.find(row_id) == id_to_rows_map.end()){
				id_to_rows_map.emplace(row_id, DataRowPointers());
			}
			id_to_rows_map[row_id].push_back(row_ptr);
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
	};
}
#endif



//, string&& value
