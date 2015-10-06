#ifndef datastream_schema_h
#define datastream_schema_h

#include <iostream>
#include <fstream>

#include "schema_set.h"
#include "datastream_definitions.h"

namespace datastream {

	using std::ifstream;

	class Schema{

	public:

		void clear(){
			schema_set_ptrs.clear();
			schema_sets.clear();
		}

		void load(
			const string & schema_sets_filename,
			const string & schema_elements_filename
		){
			loadSchemaSets(schema_sets_filename);
			mapSchemaSets();
			loadSchemaElements(schema_elements_filename);
			weaveSchema();
		}

		SchemaSets& getSchemaSets(){
			return schema_sets;
		}

	private:

		SchemaSets schema_sets;
		SchemaSetPointers schema_set_ptrs;

		void loadSchemaSets(const string & schema_sets_filename){

			clear();

			ifstream file (schema_sets_filename);
		    if ( !file.is_open()){
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
				//schema_sets.push_back(SchemaSet(etc....))

				schema_sets.emplace_back(
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

		// map
		// schema sets are stored in list container
		// this guarantees items will not be moved
		// and pointers to these objects will reliable
		// but it does not permit us fast random access
		// for this purpose a map of pointers is created
		//
		// this should always we quick, not enough rows to be focus of optimisation
		// but could try re factoring so it can occur after loadElementSchema
		// to end database connection sooner
		//
		// cyclic references are prevented because tree mapped from child to parent
		// but read parent to child.
		// as root is not mapped, any cyclic references are not traversed

		void mapSchemaSets(){

			for (
				auto
				schema_set_it = schema_sets.begin();
				schema_set_it != schema_sets.end();
				++schema_set_it
			){
				schema_set_ptrs.emplace(
					int(schema_set_it->id),
					SchemaSetPointer(&(*schema_set_it))
				);
			}
		}

		void loadSchemaElements(const string & schema_elements_filename){

			//std::cout << schema_elements_filename  << "\n";

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

				auto datatype_lookup = element_datatype_map.find(matched[match_index_element_datatype]);

				if (datatype_lookup == element_datatype_map.end()){
				 	throw std::domain_error("sorry. data cannot be understood. the type of data is not recognised.");
				}

				ElementDatatype element_datatype = datatype_lookup->second;

				int element_set_id = std::stoi(matched[match_index_element_set]);
				auto set_search = schema_set_ptrs.find(element_set_id);

				if ( set_search == schema_set_ptrs.end()){
					throw std::domain_error("sorry. data is incomplete and cannot be understood. ");
				}

				// std::cout << " ------------------\n";
				// std::cout << line << "\n";
				//
				// for (int i = 0; (i < matched.length()) && (i <= 10) ; ++i){
				// 	std::cout << i << ": " << matched[i]  << "\n";
				// }

				set_search->second->addElement(
					std::stoi(matched[match_index_element_id]),
					element_set_id,
					std::move(matched[match_index_element_name]),
					element_datatype
				);

				element_count++;
			}
			file.close();
		}

		void weaveSchema(){
			// this connects each schema set to its parent
			// use int set->parent
			// to locate parent in
			// schema_set_ptrs
			// then push child ptr into
			// parents->child_sets vector
			for(
				auto schema_set_it = schema_sets.begin();
				schema_set_it != schema_sets.end();
				++schema_set_it
			) {

				// root has no parent - do not weave
				if (schema_set_it->isRoot()){
					continue;
				}

				int parent_set_id = schema_set_it->parent;

				if (schema_set_ptrs.find(parent_set_id) == schema_set_ptrs.end()){
					throw std::domain_error("the data schema is not valid, parent not found");
				}
				schema_set_ptrs[parent_set_id]->nestChildSet(&(*schema_set_it));
			}
		}
	};
}

#endif
