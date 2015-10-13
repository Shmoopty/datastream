#ifndef datastream_data_h
#define datastream_data_h

#include "schema_set.h"
#include "data_set.h"
#include "datastream_definitions.h"
#include "formatter.h"

namespace datastream {

	class Data{

	public:

		void clear(){
			data_set_ptr_map.clear();
			data_set_list.clear();
		}

		void build(Schema& schema){
			load(schema.schemaSets());
			map(); //schema.schemaSets());
			connect();
		}

		void write(ostream & os, Formatter& formatter ) const {

			// no data loaded
			//throw error?
			if (!data_set_ptr_map.size()){
				return;
			}

			auto root_set = std::find_if(
				data_set_list.begin(),
				data_set_list.end(),
				[](const DataSet& d){
					return d.isRoot();
				}
			);

			if (root_set == data_set_list.end()){
				throw std::domain_error("cannot find root element to begin writing");
			}

			//only support one root for now
			root_set->write(os, formatter);
		}

	private:

		list<DataSet> data_set_list;
		map<int, DataSet*> data_set_ptr_map;

		void load(const list<SchemaSet>& schema_set_list){

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

		//with a little work we can
		// combine map and connect
		// and get rid of a member variable conatiner in data_row

		// to do this we must understand the dependancies in the
		// set graph

		// first is it cyclic?
		// http://www.geeksforgeeks.org/detect-cycle-in-a-graph/
		// throw if it is!

		// then perform a topological sort
		// https://en.wikipedia.org/wiki/Topological_sorting

		// see schema.h
		void map(){ //} const list<SchemaSet>& schema_set_list){

			for (DataSet& data_set : data_set_list){

				//map set rows
				data_set.mapRows();

				//map sets
				data_set_ptr_map.emplace(
					data_set.id(),
					&data_set
				);
			}
		}

		void connect(){
			for (DataSet& data_set : data_set_list){
				//root has no parent and does not need to be woven into tree
				if(data_set.isRoot()){
					continue;
				}

				auto parent_search = data_set_ptr_map.find(data_set.parent());
				if ( parent_search == data_set_ptr_map.end()){
					throw std::domain_error("data structure is invalid. parent unknown");
				}
				data_set.connect(*parent_search->second);
			}
		}
	};
}
#endif
