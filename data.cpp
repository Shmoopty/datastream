#include "data.h"

namespace datastream {

	void Data::clear(){
		data_set_ptr_map.clear();
		data_set_list.clear();
	}

	void Data::build(const Schema& schema){
		load(schema.schemaSets());
		map(schema); //schema.schemaSets());
	}

	void Data::write(ostream & os, Formatter& formatter, const Schema& schema ) const {

		// no data loaded
		//throw error?
		if (!data_set_ptr_map.size()){
			return;
		}

		// no graph loaded
		//throw error?
		if(!schema.dependencyGraph().size()){
			return;
		}

		int root_id = schema.dependencyGraph().at(0);

		auto root_search = data_set_ptr_map.find(root_id);

		if (root_search == data_set_ptr_map.end()){
			throw std::domain_error("sorry, I cannot find the beginning of the data to begin writing");
		}

		//only support one root for now
		root_search->second->write(os, formatter);
	}

	void Data::load(const list<SchemaSet>& schema_set_list){

		clear();

		for(const SchemaSet& schema_set : schema_set_list){

			ifstream file (schema_set.inputFileName());

			if (!file.is_open()){
				throw std::domain_error("cannot open schema file");
			}

			data_set_list.emplace_back(
				schema_set
			);

			string line;
			while (std::getline(file, line)){

				if(isBlank(line) || isComment(line)){
				 	continue;
				}

				// current data set is last added,
				// load rows
				data_set_list.rbegin()->load(schema_set.childElements(), line);
			}
			file.close();
		}
	}

	void Data::map(const Schema& schema){

		//map sets
		for (DataSet& data_set : data_set_list){
			//map sets
			data_set_ptr_map.emplace(
				data_set.id(),
				&data_set
			);
		}

		//map rows
		auto& dependency_graph = schema.dependencyGraph();

		for ( int set_id : dependency_graph){

			auto set_search = data_set_ptr_map.find(set_id);

			if ( set_search == data_set_ptr_map.end()){
				throw std::domain_error("sorry, the data cannot be understood : a section is missing.");
			}

			DataSet* parent_set_ptr = nullptr;

			if (set_search->second->hasParent()){
				auto parent_search = data_set_ptr_map.find(set_search->second->parent());
				if ( parent_search == data_set_ptr_map.end()){
					throw std::domain_error("data structure is invalid. parent unknown");
				}
				parent_set_ptr = parent_search->second;
			}

			//map set rows
			set_search->second->mapRows(parent_set_ptr);
		}
	}
}
