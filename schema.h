#ifndef datastream_schema_h
#define datastream_schema_h

#include <boost/iterator/transform_iterator.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/sequenced_index.hpp>
#include <boost/multi_index/composite_key.hpp>
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

	/* Drew Dormann - 
		(Temporarily?) here to make the multi_indexed_container easier to read */
	using namespace boost::multi_index;
	
	class Schema{

	public:

		/* Drew Dormann - 
			These structs  are completely optional, and simply allow an
			indexing interface to have a (compile-time) readable name.
			E.g.: container.get<indexed_by_id>() instead of container.get<1>().

			 The structs and the lines that use them below could be removed. */
		
		struct indexed_by_id{};
		struct ordered_by_parent_and_position{};

		/* Drew Dormann - 
			Here's the fast rundown on a multi-indexed container.  A powerful
		beast.
		 
		This declaration declares the contained object as its first template
		parameter - same as any standard container.  The second "indexed_by"
		parameter is a compile-time list of interfaces that can be used on the
		container, accessed by member variable get<0-based index>() or
		get<tag name>().  The first index's interface is also mapped onto
		the container as a whole, so get<0>() is never needed.
		
		Our first index iterates the objects in the order they're pushed back.
		Random-access is possible, but we're using "sequenced<>", which behaves
		like a std::list, since that's all we need.
		
		The second index implements a set, ordered by the value returned from
		SchemaSet::id().
		
		The third index also behaves like a set, but ordered from (effectively)
		std::pair< (parent), (position) >.
		  
		*/
		
		using Sets = boost::multi_index_container<
			SchemaSet,
			indexed_by<
				// Sequenced, like a std::list
				sequenced<>,

				// Ordered by id, like std::set
				ordered_unique<
					tag<indexed_by_id>,
					const_mem_fun<SchemaSet,unsigned,&SchemaSet::id>
				>,

				// Ordered by parent + position, like std::set
				ordered_unique<
					tag<ordered_by_parent_and_position>,
					composite_key<
						SchemaSet,
						const_mem_fun<SchemaSet,unsigned,&SchemaSet::parent>,
						const_mem_fun<SchemaSet,unsigned,&SchemaSet::position>
					>
				>
			>
		>;
		void build(
			const string & schema_sets_filename,
			const string & schema_elements_filename
		);

		inline const Sets & sets() const { return mi_sets_;}
		inline const vector<int> & dependencyOrder() const {return dependency_order_;}

		//not required for tree output as writing begins from root
		//may be usefule for flat output
		//inline const map<int, SchemaSet*> & schemaSetsById() { return sets_by_id_;}

	private:

		Sets mi_sets_;
		vector<int> dependency_order_;

		void clear();
		void loadSets(const string & schema_sets_filename);
		void mapSets();
		void connect();
		void loadElements(const string & schema_elements_filename);
	};
}

#endif
