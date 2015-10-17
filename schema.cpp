#include "schema.h"
#include "datastream_definitions.h"

namespace datastream {

	using std::ifstream;
	using std::sort;
	using std::set;

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
		sets_by_id_.clear();
		sets_.clear();
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

			sets_.emplace_back(
				sets_.size(),
				std::stoi(matched[match_index_schema_id]),
				std::stoi(matched[match_index_schema_parent]),
				std::move(matched[match_index_group_name]),
				std::move(matched[match_index_row_name]),
				std::move(matched[match_index_input_filename]),

				(bool)std::stoi(matched[match_index_hide_when_empty]),
				(bool)std::stoi(matched[match_force_single_row_per_parent]),
				group_wrapper,
				row_wrapper
			);
		}
		file.close();
	}

	void Schema::mapSets()
	{
		/*
			copy pointers to a vector for sorting & processing
		*/
		//copy pointers to temporary vector
		vector <SchemaSet*> set_ptrs;
		set_ptrs.reserve(sets_.size());

		for ( SchemaSet& schema_set : sets_ ){

			set_ptrs.emplace_back(
				&schema_set
			);
		}

		/*
			create row id : * map
		*/
		//sort by id for fast insert into map
		sort(
			set_ptrs.begin(),
			set_ptrs.end(),
			[](SchemaSet * a,  SchemaSet * b)->bool{
				return a->id() < b->id();
			}
		);

		//insert pairs into map
		for ( SchemaSet* set_ptr : set_ptrs ){
			sets_by_id_.emplace_hint(
				sets_by_id_.end(),
				int(set_ptr->id()),
				set_ptr
			);
		}

		/*
			validate the structure of connected set
			it must form a tree:
				it must have one root
				it must be acyclic
				all nodes must be connected
		*/

		// test 1:
		// does structure have one root?
		//find a quick way to do this on std::find_if iterator and this can be cut
		int root_count = std::count_if(
			sets_.begin(),
			sets_.end(),
			[](const SchemaSet& set){
				return !set.hasParent();
			}
		);

		if (root_count != 1){
			throw std::domain_error("sorry, the data does not contain a starting point to begin writing");
		}

		//locate the root
		auto root_search = std::find_if(
			sets_.begin(),
			sets_.end(),
			[](const SchemaSet& set){
				return !set.hasParent();
			}
		);

		if (root_search == sets_.end()){
			throw std::domain_error("sorry about this, i cannot find the starting point to begin writing.");
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

		// step 1
		// reuse vector
		// sort by parent id, original position
		*/
		std::sort(
			set_ptrs.begin(),
			set_ptrs.end(),
			[](const SchemaSet* a, const SchemaSet* b){
				if (a->parent() == b->parent()){
					return a->position() < b->position();
				}
				return a->parent() < b->parent();
			}
		);

		/*
			//step 2
			insert records into the parent row id : * map
		*/
		std::map<unsigned int, std::vector<unsigned int>> set_ids_by_parent;

		for (SchemaSet* set_ptr : set_ptrs){

			if (!set_ptr->hasParent()){
				continue;
			}

			auto set_ids_by_parent_find = set_ids_by_parent.find(set_ptr->parent());

			if (set_ids_by_parent_find == set_ids_by_parent.end()){
				set_ids_by_parent.emplace_hint(
					set_ids_by_parent.end(),
					set_ptr->parent(),
					std::vector<unsigned int>{set_ptr->id()}
				);
			}
			else {
				set_ids_by_parent_find->second.push_back(set_ptr->id());
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
		dependency_order_.reserve(set_ptrs.size());
		walk(root_search->id());

		// validate step 3
		// this should be redundant given that we have checked for multiple roots
		// check that all nodes have been visited by walk
		if (dependency_order_.size() != set_ptrs.size()){
			throw std::domain_error("sorry about this, the data doesn't seem to be connected into a single document");
		}
		connect();
	}

	void Schema::connect(){

		// for schema using dependancy graph for connect has no advantage
		// will be useful when connecting data row

		for ( SchemaSet& schema_set : sets_ ){

			if (!schema_set.hasParent()){
				continue;
			}

			auto schema_set_by_id_search = sets_by_id_.find(schema_set.parent());
			if ( schema_set_by_id_search == sets_by_id_.end()){
				throw std::domain_error("sorry, the data cannot be understood : a section is missing.");
			}
			schema_set_by_id_search->second->connect(schema_set);
		}
	}

	void Schema::loadElements(const string & schema_elements_filename)
	{
		ifstream file (schema_elements_filename);
	    if ( !file.is_open()){
			throw std::domain_error("cannot open element schema.");
		}

		unsigned int element_count = 0;

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
			auto set_search = sets_by_id_.find(element_set_id);

			if ( set_search == sets_by_id_.end()){
				throw std::domain_error("sorry. data is incomplete and cannot be understood. ");
			}

			set_search->second->addElement(
				std::stoi(matched[match_index_element_id]),
				element_set_id,
				std::move(matched[match_index_element_name]),
				element_data_type
			);

			element_count++;
		}
		file.close();
	}
}
