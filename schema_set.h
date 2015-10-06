#ifndef datastream_schema_set_h
#define datastream_schema_set_h

#include <string>
#include <vector>
#include <list>
#include <map>
#include <iostream>

#include "schema_element.h"

namespace datastream {

	using std::string;
	using std::vector;
	using std::list;
	using std::map;

	class SchemaSet;

	//change vector to list
	typedef list<SchemaSet> SchemaSets;
	typedef SchemaSet* SchemaSetPointer;
	typedef map<int, SchemaSetPointer> SchemaSetPointers;

	const unsigned int root_set_index = 0;

	class SchemaSet {

		friend class Schema;
		friend class DataRow;
		friend class DataSet;
		friend class Data;

	public:

		SchemaSet(
			bool root,
			unsigned int id,
			unsigned int parent,
			string&& group_name,
			string&& row_name,
			string&& filename,

			// advanced features
			// use at your peril
			bool limit_single_child = false,
			bool hide_when_empty = false,

			// expect that when used for json
			// schema validator will require:

			// hide_when_empty to true
			// when
			// SetWrappper : no_wrapper

			GroupWrapper groupWrapper = GroupWrapper::array_wrapper,


			// expect that when used for json or xml
			// schema validator will require:

			// limit_single_child to true
			// when
			// rowWrapper : no_wrapper

			RowWrapper rowWrapper = RowWrapper::object_wrapper

		):
			root(root),
			id(id),
			parent(parent),
			group_name(group_name),
			row_name(row_name),
			input_filename (filename),

			// some day we are going to need this.
			// but if you use it you need to be aware
			// of the possible consequences

			hide_when_empty(hide_when_empty),
			limit_single_child(limit_single_child),

			groupWrapper (groupWrapper),
			rowWrapper (rowWrapper)

		{
		};

		bool getId(){
			return id;
		};

		void nestChildSet(SchemaSet* child_set){
			child_sets.push_back(child_set);
		}

		void addElement(int element_id, int element_parent, string&& element_name, ElementDatatype element_datatype){

			child_elements.emplace_back(
				element_id,
				element_parent,
				std::move(element_name),
				element_datatype
			);
		}

		bool isRoot(){
			return root;
		}

	private:
		bool root;
		unsigned int id;
		unsigned int parent;
		string group_name;
		string row_name;
		string input_filename;

		// advanced features
		// if you use them
		// you will most likely regret it

		bool hide_when_empty;
		bool limit_single_child;
		GroupWrapper groupWrapper;
		RowWrapper rowWrapper;

		list<SchemaElement> child_elements;
		vector<SchemaSet*> child_sets;
	};
};

#endif
