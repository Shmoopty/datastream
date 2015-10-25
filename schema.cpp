#include "schema.h"
#include "datastream_definitions.h"

namespace datastream {

	using std::ifstream;
	using std::sort;
	//using std::set;

	void Schema::build(
		const string & schema_sets_filename,
		const string & schema_elements_filename
	){
		clear();
		loadSets(schema_sets_filename);
		mapSets();
		loadElements(schema_elements_filename);
	}

	void Schema::clear()
	{
		mi_sets_.clear();
	}

	void Schema::loadSets(const string & schema_sets_filename){

		ifstream file (schema_sets_filename);
		if (!file.is_open()){
			throw std::domain_error("cannot open schema.");
		}

		string line;
		while (std::getline(file, line)){

			if(isBlank(line) || isComment(line)){
				continue;
			}

			std::smatch matched;
			std::regex_match(line, matched, schema_set_line_pattern);

			if (!matched.length()){
				throw std::domain_error("schema format does not match expected pattern.");
			}

			auto group_wrapper_lookup = group_wrapper_map.find(matched[match_group_wrapper_type]);
			if (group_wrapper_lookup == group_wrapper_map.end()){
				throw std::domain_error("data does not match expected format. : group wrapper not understood");
			}
			GroupWrapper group_wrapper = group_wrapper_lookup->second;

			auto row_wrapper_lookup = row_wrapper_map.find(matched[match_row_wrapper_type]);
			if (row_wrapper_lookup == row_wrapper_map.end()){
				throw std::domain_error("data does not match expected format. : row wrapper not understood");
			}
			RowWrapper row_wrapper = row_wrapper_lookup->second;

			mi_sets_.push_back(SchemaSet(
				mi_sets_.size(),
				std::stoi(matched[match_index_schema_id]),
				std::stoi(matched[match_index_schema_parent]),
				std::move(matched[match_index_group_name]),
				std::move(matched[match_index_row_name]),
				std::move(matched[match_index_input_filename]),

				/* Drew Dormann -
					Not a functional change, but C-style casts are very often frowned upon */
				static_cast<bool>(std::stoi(matched[match_index_hide_when_empty])),
				static_cast<bool>(std::stoi(matched[match_force_single_row_per_parent])),
				group_wrapper,
				row_wrapper
			));
		}
		/* Drew Dormann -
			Only the EOF state is acceptable here. */
		if (!file.eof())
		{
			throw std::runtime_error("file operation failure while reading " + schema_sets_filename);
		}
		file.close();
	}

	void Schema::mapSets()
	{
		/*
			copy pointers to a vector for sorting & processing
		*/

		/*
			validate the structure of connected set
			it must form a tree:
				it must have one root
				it must be acyclic
				all nodes must be connected
		*/
		/* Drew Dormann -
			The changes below only iterate the set once.  Instead of
			"count, then find", this does "find, then continue the same find". */

		// test 1:
		// locate the root
		auto root_search = std::find_if(
			mi_sets_.begin(),
			mi_sets_.end(),
			[](const SchemaSet& set){
				return !set.hasParent();
			}
		);

		if (root_search == mi_sets_.end()){
			throw std::domain_error("sorry about this, i cannot find the starting point to begin writing");
		}

		/* Drew Dormann -
			The "find_if" below could be changed to "count_if", should some
			future need - like an error message - require the actual number
			of roots detected. */

		// validate that root_search is the only root
		auto remaining_sets_begin = root_search;
		++remaining_sets_begin;
		auto multiple_roots_search = std::find_if(
			remaining_sets_begin,
			mi_sets_.end(),
			[](const SchemaSet& set){
				return !set.hasParent();
			}
		);

		if (multiple_roots_search != mi_sets_.end()){
			throw std::domain_error("sorry, the data does not contain one starting point to begin writing.");
		}

		// now we have a root make a map of parent to child to begin
		// cyclic check and topographical sort
		// why do this now and not just walk the tree when we are ready to write
		// this means we find the error quicker
		// and means we can flatten the writing loop
		// sometimes that can save on memory required when compared to
		// a recursive algorithm
		// will that matter here?
		// guess it depend on how many levels we nest
		// but it probably won't hurt

		/*
			create parent : * map

			//step 1
			insert records into the parent row id : * map
		*/
		std::map<unsigned int, std::vector<unsigned int>> set_ids_by_parent;

		for (const SchemaSet& set : mi_sets_.get<ordered_by_parent_and_position>() ){

			if (!set.hasParent()){
				continue;
			}

			auto set_ids_by_parent_find = set_ids_by_parent.find(set.parent());

			if (set_ids_by_parent_find == set_ids_by_parent.end()){
				set_ids_by_parent.emplace_hint(
					set_ids_by_parent.end(),
					set.parent(),
					std::vector<unsigned int>{set.id()}
				);
			}
			else {
				set_ids_by_parent_find->second.push_back(set.id());
			}
		}

		/*
			validate structure
			// step 2
			// walk the tree
			// look for cycles

			while walking the tree build dependancy graph
		*/
		// recursive lambda to walk the tree and validate structure
		std::function<void(unsigned int)> walk = [&](unsigned int set_id){

			//cyclic dependancy check
			if (
				std::find(
					dependency_order_.begin(),
					dependency_order_.end(),
					set_id
				) != dependency_order_.end()
			){
				//oh dear this doesn't look good
				throw std::domain_error("sorry about this, the data seems to be connected in an endless loop");
			}

			dependency_order_.push_back(set_id);

		  	auto child_ids_search = set_ids_by_parent.find(set_id);

			if (child_ids_search != set_ids_by_parent.end()){
				for (int child_id : child_ids_search->second){
					walk(child_id);
				}
			}
		};

		// now use the lambda,
		// walk the tree populating dependency graph
		dependency_order_.reserve(mi_sets_.size());
		walk(root_search->id());

		// validate step 3
		// this should be redundant given that we have checked for multiple roots
		// check that all nodes have been visited by walk
		if (dependency_order_.size() != mi_sets_.size()){

			//DEV
			// TEMP throw std::domain_error("sorry about this, the data doesn't seem to be connected into a single document");
			throw std::domain_error("sorry about this, dependency_order_.size() " + std::to_string(dependency_order_.size()) +
				" doesn't seem to match mi_sets_.size() " + std::to_string(mi_sets_.size()) );
		}
		connect();
	}

	void Schema::connect(){

		// for schema using dependancy graph for connect has no advantage
		// will be useful when connecting data row

		auto& sets_by_id = mi_sets_.get<indexed_by_id>();

		for ( const SchemaSet& schema_set : mi_sets_ ){

			if (!schema_set.hasParent()){
				continue;
			}

			auto schema_set_by_id_search = sets_by_id.find(schema_set.parent());
			if ( schema_set_by_id_search == sets_by_id.end()){
				throw std::domain_error("sorry, the data cannot be understood : a section is missing.");
			}
			sets_by_id.modify( schema_set_by_id_search, [&]( SchemaSet& set ){ set.connect(schema_set); } );
		}
	}

	void Schema::loadElements(const string & schema_elements_filename)
	{
		ifstream file (schema_elements_filename);
		if ( !file.is_open()){
			throw std::domain_error("cannot open element schema.");
		}

		unsigned int element_count = 0;
		auto& sets_by_id = mi_sets_.get<indexed_by_id>();

		string line;
		while (std::getline(file, line)){

			if(isBlank(line) || isComment(line)){
				continue;
			}

			std::smatch matched;
			std::regex_match(line, matched, schema_element_line_pattern);

			if (!matched.length()){
				throw std::domain_error("element schema format does not match expected pattern.");
			}

			auto data_type_lookup = element_data_type_map.find(matched[match_index_element_data_type]);

			if (data_type_lookup == element_data_type_map.end()){
			 	throw std::domain_error("sorry. data cannot be understood. the type of data is not recognised.");
			}

			ElementDataType element_data_type = data_type_lookup->second;

			int element_set_id = std::stoi(matched[match_index_element_set]);
			auto set_search = sets_by_id.find(element_set_id);

			if ( set_search == sets_by_id.end()){
				throw std::domain_error("sorry. data is incomplete and cannot be understood. ");
			}

			sets_by_id.modify( set_search, [&]( SchemaSet& set ){
				set.addElement(
					std::stoi(matched[match_index_element_id]),
					element_set_id,
					std::move(matched[match_index_element_name]),
					element_data_type
				);
			} );

			element_count++;
		}
		/* Drew Dormann -
			Only the EOF state is acceptable here. */
		if (!file.eof())
		{
			throw std::runtime_error("file operation failure while reading " + schema_elements_filename);
		}
		file.close();
	}
}
