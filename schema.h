#ifndef datastream_schema_h
#define datastream_schema_h

#include <boost/iterator/transform_iterator.hpp>
#include <algorithm>
#include <functional>

#include <iostream>
#include <fstream>
#include <utility>
#include <set>

#include "schema_set.h"
#include "datastream_definitions.h"

namespace datastream {

	using std::ifstream;
	using std::sort;
	using std::set;

	class Schema{

	public:

		void build(
			const string & schema_sets_filename,
			const string & schema_elements_filename
		);

		inline const list<SchemaSet> & sets() const { return sets_;}
		inline const vector<int> & dependencyOrder() const {return dependency_order_;}

		//not required for tree output as writing begins from root
		//may be usefule for flat output
		//inline const map<int, SchemaSet*> & schemaSetsById() { return sets_by_id_;}

	private:

		list<SchemaSet> sets_;
		map<int, SchemaSet*> sets_by_id_;
		vector<int> dependency_order_;

		void clear();
		void loadSets(const string & schema_sets_filename);
		void mapSets();
		void connect();
		void loadElements(const string & schema_elements_filename);
	};
}

#endif
