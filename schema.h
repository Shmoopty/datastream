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

		inline const list<SchemaSet> & schemaSets() const { return schema_set_list_;}
		inline const vector<int> & dependencyGraph() const {return dependency_graph_;}
		inline const map<int, SchemaSet*> & schemaSetsMap() { return schema_set_ptr_map;}

	private:

		list<SchemaSet> schema_set_list_;
		map<int, SchemaSet*> schema_set_ptr_map;
		vector<int> dependency_graph_;

		inline const list<SchemaSet>& sets() const
		{
			return schema_set_list_;
		}

		void clear();
		void loadSets(const string & schema_sets_filename);
		void mapSets();
		void connect();
		void loadElements(const string & schema_elements_filename);
	};
}

#endif
