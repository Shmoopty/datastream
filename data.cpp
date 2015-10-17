#include "data.h"

namespace datastream {

	void Data::clear(){
		sets_by_id_.clear();
		sets_.clear();
	}

	void Data::build(const Schema& schema){
		load(schema.sets());
		map(schema);
	}

	void Data::write(ostream & os, Formatter& formatter, const Schema& schema ) const {

		// no data loaded
		//throw error?
		if (!sets_by_id_.size()){
			return;
		}

		// no graph loaded
		//throw error?
		if(!schema.dependencyOrder().size()){
			return;
		}

		int root_id = schema.dependencyOrder().at(0);

		auto root_search = sets_by_id_.find(root_id);

		if (root_search == sets_by_id_.end()){
			throw std::domain_error("sorry, I cannot find the beginning of the data to begin writing");
		}

		//only support one root for now
		root_search->second->write(os, formatter);
	}

	void Data::load(const list<SchemaSet>& schema_sets){

		clear();

		for(const SchemaSet& schema_set : schema_sets){

			ifstream file (schema_set.inputFileName());

			if (!file.is_open()){
				throw std::domain_error("cannot open schema file");
			}

			sets_.emplace_back(
				schema_set
			);

			string line;
			while (std::getline(file, line)){

				if(isBlank(line) || isComment(line)){
				 	continue;
				}

				// current data set is last added,
				// load rows
				sets_.rbegin()->load(schema_set.childElements(), line);
			}
			file.close();
		}
	}

	void Data::map(const Schema& schema){

		//map sets_
		for (DataSet& data_set : sets_){
			//map sets_
			sets_by_id_.emplace(
				data_set.id(),
				&data_set
			);
		}

		//map rows
		auto& dependency_order = schema.dependencyOrder();

		for ( int set_id : dependency_order){

			auto set_search = sets_by_id_.find(set_id);

			if ( set_search == sets_by_id_.end()){
				throw std::domain_error("sorry, the data cannot be understood : a section is missing.");
			}

			DataSet* parent_set_ptr = nullptr;

			if (set_search->second->hasParent()){
				auto parent_search = sets_by_id_.find(set_search->second->parent());
				if ( parent_search == sets_by_id_.end()){
					throw std::domain_error("data structure is invalid. parent unknown");
				}
				parent_set_ptr = parent_search->second;
			}

			//map set rows
			set_search->second->mapRows(parent_set_ptr);
		}
	}
}
