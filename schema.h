#ifndef datastream_schema_h
#define datastream_schema_h

#include <boost/iterator/transform_iterator.hpp>
#include <algorithm>
#include <functional>

#include <iostream>
#include <fstream>
#include <utility>

#include "schema_set.h"
#include "datastream_definitions.h"

namespace datastream {

	using std::ifstream;
	using std::sort;

	class Schema{

	public:

		void clear()
		{
			schema_set_ptr_map.clear();
			schema_set_list.clear();
		}

		void load(
			const string & schema_sets_filename,
			const string & schema_elements_filename
		){
			loadSets(schema_sets_filename);
			map();
			loadElements(schema_elements_filename);
			connect();
		}

		list<SchemaSet>& getSets()
		{
			return schema_set_list;
		}

	private:

		list<SchemaSet> schema_set_list;
		map<int, SchemaSet*> schema_set_ptr_map;

		void loadSets(const string & schema_sets_filename){

			clear();

			ifstream file (schema_sets_filename);
		    if (!file.is_open()){
				throw std::domain_error("cannot open schema.");
			}

			bool rootFound = false;

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

				bool is_root = (bool)std::stoi(matched[match_index_is_root]);
				if (is_root){
					if (rootFound){
						throw std::domain_error("data does not match expected format. : document root defined multiple times");
					}
					rootFound = true;
				}

				//downgrade for c++ 03
				//schema_set_list.push_back(SchemaSet(etc....))

				schema_set_list.emplace_back(
					is_root,
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

			if (!rootFound){
				throw std::domain_error("data does not match expected format. : document root not specified");
			}

		}

		void map()
		{
			//copy pointers to temporary vector
			vector <SchemaSet*> schema_set_ptrs;
			schema_set_ptrs.reserve(schema_set_list.size());

			for ( SchemaSet& schema_set : schema_set_list ){
				schema_set_ptrs.emplace_back(
					&schema_set
				);
			}

			//sort by id for fast insert into map
			sort(
				schema_set_ptrs.begin(),
				schema_set_ptrs.end(),
				[](SchemaSet * a,  SchemaSet * b)->bool{
					return a->id < b->id;
				}
			);

 			//insert pairs into map
			for ( SchemaSet* schema_set_ptr : schema_set_ptrs ){
				schema_set_ptr_map.emplace_hint(
					schema_set_ptr_map.end(),
					int(schema_set_ptr->id),
					schema_set_ptr
				);
			}
		}

		void loadElements(const string & schema_elements_filename)
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
				auto set_search = schema_set_ptr_map.find(element_set_id);

				if ( set_search == schema_set_ptr_map.end()){
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

		void connect(){

			// this connects each schema set to its parent
			for ( SchemaSet& schema_set : schema_set_list ){

				// root has no parent - do not connect
				if (schema_set.isRoot()){
					continue;
				}

				int parent_set_id = schema_set.parent;

				auto schema_set_ptr_map_search = schema_set_ptr_map.find(parent_set_id);
				if ( schema_set_ptr_map_search == schema_set_ptr_map.end()){
					throw std::domain_error(error_text_missing_parent);
				}
				schema_set_ptr_map_search->second->connect(schema_set);
			}
		}
	};
}

#endif
