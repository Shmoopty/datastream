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

			bool limit_single_child = false,
			bool hide_when_empty = false,

			GroupWrapper groupWrapper = GroupWrapper::array_wrapper,
			RowWrapper rowWrapper = RowWrapper::object_wrapper

		):
			root(root),
			id(id),
			parent(parent),
			group_name(group_name),
			row_name(row_name),
			input_filename (filename),

			hide_when_empty(hide_when_empty),
			limit_single_child(limit_single_child),

			groupWrapper (groupWrapper),
			rowWrapper (rowWrapper)
		{};

		bool getId()
		{
			return id;
		};

		void connect(SchemaSet& child_set)
		{
			child_sets.push_back(&child_set);
		}

		void addElement(int element_id, int element_parent, string&& element_name, ElementDataType element_data_type)
		{
			child_elements.emplace_back(
				element_id,
				element_parent,
				std::move(element_name),
				element_data_type
			);
		}

		bool isRoot()
		{
			return root;
		}

	private:
		bool root;
		unsigned int id;
		unsigned int parent;
		string group_name;
		string row_name;
		string input_filename;

		bool hide_when_empty;
		bool limit_single_child;
		GroupWrapper groupWrapper;
		RowWrapper rowWrapper;

		list<SchemaElement> child_elements;
		vector<SchemaSet*> child_sets;
	};
};

#endif
