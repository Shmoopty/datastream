#include "data_set.h"

namespace datastream {

	DataSet::DataSet(const SchemaSet& schema_set):
	schema_set(schema_set)
	{};

	void DataSet::load(const list<SchemaElement> & schema_elements, const string & line)
	{
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

	// rows are stored in a list to give us a stable address
	// now put pointers to rows into vector
	// for sorting and random access

	void DataSet::mapRows(DataSet* parent_set_ptr)
	{
		//push pointer to rows from list into vector
		vector<DataRow*> row_ptrs;

		for(DataRow& row : rows )
		{
			row_ptrs.push_back(&row);
		}

		if(schema_set.hasChildSets()){

			//sort vector by row id
			std::sort(
				row_ptrs.begin(),
				row_ptrs.end(),
				[](const DataRow* a, const DataRow* b){
					if (a->id() == b->id())
					{
						return a->position() < b->position();
					}
					return a->id() < b->id();
				}
			);

			// map of vectors handles multiple appearances of same id
			for (DataRow* row_ptr : row_ptrs){

				if (id_to_rows_map.find(row_ptr->id()) == id_to_rows_map.end()){
					id_to_rows_map.emplace_hint(
						id_to_rows_map.end(),
						row_ptr->id(),
						vector<DataRow*>{row_ptr});
				}
				else{
					id_to_rows_map[row_ptr->id()].push_back(row_ptr);
				}
			}
		}

		if(schema_set.hasParent() && parent_set_ptr != nullptr){

			//reuse vector
			//sort vector by row parent
			std::sort(
				row_ptrs.begin(),
				row_ptrs.end(),
				[](const DataRow* a, const DataRow* b){
					if (a->parent() == b->parent())
					{
						return a->position() < b->position();
					}
					return a->parent() < b->parent();
				}
			);

			/*
				create a parent : vector of *  map
			*/
			std::map<int, std::shared_ptr<std::vector<DataRow*>>> rows_by_parent_map;

			for (DataRow* row_ptr : row_ptrs){

				auto rows_by_parent_map_search = rows_by_parent_map.find(row_ptr->parent());

		/* Drew Dormann -
			It's a shame that "std::make_shared" doesn't recognize initilizer lists
			unless they're explicit.  The original code here wasn't exception-
			unsafe in its current form, but this change reduces 2 heap allocs to
			one.  C++ also seems to be evolving away from raw "new" usage when possible. */
				if (rows_by_parent_map_search == rows_by_parent_map.end()){
					rows_by_parent_map.emplace_hint(
						rows_by_parent_map.end(),
						row_ptr->parent(),
						std::make_shared<std::vector<DataRow*>>(std::initializer_list<DataRow*>{row_ptr})
					);
				}
				else {
					rows_by_parent_map_search->second.get()->push_back(row_ptr);
				}
			}

			// now nest rows into parent by passing a shared pointer
			// to any matching parent rows
			for (auto& parent_id_child_rows : rows_by_parent_map){

				auto parent_rows_search = parent_set_ptr->id_to_rows_map.find(parent_id_child_rows.first);

				if(parent_rows_search != parent_set_ptr->id_to_rows_map.end()){

					auto parent_rows = parent_rows_search->second;
					for(DataRow* parent_row : parent_rows)
					{
						parent_row->connect(id(), parent_id_child_rows.second);
					}
				}
			}
		}
	}

	void DataSet::write (ostream & os, Formatter& formatter) const
	{
		unsigned int siblings_written = 0;
		formatter.open(os, schema_set.groupWrapper());

		writeRows (
			os,
			RowWrapper::no_wrapper, //parent row wrapper - root has no parent
			schema_set, 			//schema set for rows
			rows,
			formatter,
			siblings_written
		);

		formatter.close(os, schema_set.groupWrapper());
	}
}
