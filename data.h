#ifndef datastream_data_h
#define datastream_data_h

#include "schema_set.h"
#include "data_set.h"
#include "datastream_definitions.h"
#include "formatter.h"

namespace datastream {

	class Data{

		//DataSet* root = nullptr;

	public:

		void clear(){
			data_set_ptr_map.clear();
			data_set_list.clear();
		}

		void load(Schema& schema){
			loadDataSets(schema.getSchemaSets());
			mapDataSets(schema.getSchemaSets());
			weaveData();
		}

		void write(ostream & os, Formatter& formatter ) const {

			// gentle return with no output
			// if no data loaded
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

			root_set->write(os, formatter);
		}

	private:

		list<DataSet> data_set_list;
		map<int, DataSet*> data_set_ptr_map;

		void loadDataSets(list<SchemaSet>& schema_set_list){

			clear();

			for(SchemaSet& schema_set : schema_set_list){
				loadDataSet(schema_set);
			}
		}

		void loadDataSet(SchemaSet& schema_set ){

			ifstream file (schema_set.input_filename);

			if (!file.is_open()){
				throw std::domain_error("cannot open schema file");
			}

			//refactor to data_set when moving to db code

			data_set_list.emplace_back(
				&schema_set
			);

			string line;
			while (std::getline(file, line)){

				if(isBlank(line) || isComment(line)){
				 	continue;
				}

				// current data set is last added,
				// load rows
				data_set_list.rbegin()->load(schema_set.child_elements, line);
			}
			file.close();
		}

		void mapDataSets(list<SchemaSet>& schema_set_list){

			for (DataSet& data_set : data_set_list){
				//map set rows
				data_set.mapRows();

				//map sets
				data_set_ptr_map.emplace(

					data_set.getId(),
					&data_set
				);
			}
		}

		void weaveData(){
			for (DataSet& data_set : data_set_list){
				//root has no parent and does not need to be woven into tree
				if(data_set.isRoot()){
					continue;
				}

				auto parent_search = data_set_ptr_map.find(data_set.getParentId());
				if ( parent_search == data_set_ptr_map.end()){
					throw std::domain_error("data structure is invalid. parent unknown");
				}
				data_set.weave(*parent_search->second);
			}
		}
	};
}
#endif
