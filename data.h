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
			data_set_ptrs.clear();
			data_sets.clear();
		}

		void load(Schema& schema){
			loadDataSets(schema.getSchemaSets());
			mapDataSets(schema.getSchemaSets());
			weaveData();
		}

		void write(ostream & os, Formatter& formatter ) const {

			// gentle return with no output
			// if no data loaded
			if (!data_set_ptrs.size()){
				return;
			}

			auto root_set = std::find_if(
				data_sets.begin(),
				data_sets.end(),
				[=](const DataSet& d
				) {
					return d.isRoot();
				}
			);

			if (root_set == data_sets.end()){
				throw std::domain_error("cannot find root element to begin writing");
			}

			root_set->write(os, formatter);
		}

	private:

		DataSets data_sets;
		DataSetPointersMap data_set_ptrs;

		void loadDataSets(SchemaSets& schema_sets){

			clear();

			for(
				auto schema_set_it = schema_sets.begin();
				schema_set_it != schema_sets.end();
				++schema_set_it
			){
				loadDataSet(schema_set_it);
			}
		}

		void loadDataSet(const SchemaSets::iterator & schema_set_it ){

			ifstream file (schema_set_it->input_filename);

			if (!file.is_open()){
				throw std::domain_error("cannot open  schema file");
			}

			data_sets.emplace_back(
				data_sets.size(),
				&(*schema_set_it)
			);

			//refactor to data set!!
			//iterate over rows
			string line;
			while (std::getline(file, line)){

				if(isBlank(line) || isComment(line)){
				 	continue;
				}
				//current data set is last added, add row
				data_sets.rbegin()->load(schema_set_it->child_elements, line);
			}
			file.close();
		}

		void mapDataSets(SchemaSets& schema_sets){
			for (
				auto
				data_set_it  = data_sets.begin();
				data_set_it != data_sets.end();
				data_set_it++
			)
			{
				//map set rows
				data_set_it->mapRows();

				//map sets
				data_set_ptrs.emplace(

					data_set_it->getId(),
					DataSetPointer(&(*data_set_it))
				);
			}
		}

		void weaveData(){
			for (
				auto
				data_set_it = data_sets.begin();
				data_set_it != data_sets.end();
				data_set_it++
			){
				//root has no parent and does not need to be woven into tree
				if(data_set_it->isRoot()){
					continue;
				}

				auto parent_search = data_set_ptrs.find(data_set_it->getParentId());
				if ( parent_search == data_set_ptrs.end()){
					throw std::domain_error("data structure is invalid. parent unknown");
				}
				data_set_it->weave(*parent_search->second);
			}
		}
	};
}
#endif
