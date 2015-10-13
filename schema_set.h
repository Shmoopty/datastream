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

	public:
		SchemaSet(
			unsigned int id,
			unsigned int order,
			unsigned int parent,
			string&& group_name,
			string&& row_name,
			string&& filename,

			bool limit_single_child = false,
			bool hide_when_empty = false,

			GroupWrapper groupWrapper = GroupWrapper::array_wrapper,
			RowWrapper rowWrapper = RowWrapper::object_wrapper
		):
		id_(id),
		order_(order),
		parent_(parent),
		group_name_(group_name),
		row_name_(row_name),
		input_filename_ (filename),

		hide_when_empty_ (hide_when_empty),
		limit_single_child_ (limit_single_child),

		group_wrapper_ (groupWrapper),
		row_wrapper_ (rowWrapper)
		{};

		bool hasParent() const
		{
			return !( parent_ == 0 || id_ == parent_) ;
		}

		unsigned int id() const
		{
			return id_;
		};

		unsigned int parent() const
		{
			return parent_;
		}

		const string & groupName () const { return group_name_;}
		const string & rowName () const { return row_name_;}
		const string & inputFileName() const { return input_filename_;}
		bool hideWhenEmpty() const {return hide_when_empty_;}
		bool limitSingleChild() const {return limit_single_child_;}

		RowWrapper rowWrapper () const {return row_wrapper_;}
		GroupWrapper groupWrapper () const {return group_wrapper_;}


		void connect(SchemaSet& child_set)
		{
			child_sets_.push_back(&child_set);
		}

		void addElement(int element_id, int element_parent, string&& element_name, ElementDataType element_data_type)
		{
			child_elements_.emplace_back(
				element_id,
				element_parent,
				std::move(element_name),
				element_data_type
			);
		}

		bool hasChildSets() const { return child_sets_.size() > 0;}
		//can do better than this
		const vector<SchemaSet*>& childSets() const {return child_sets_;}
		const list<SchemaElement>& childElements() const {return child_elements_;}
		unsigned int order() const {return order_;}

	private:

		//bool root_;
		unsigned int id_;
		unsigned int order_;
		unsigned int parent_;
		string group_name_;
		string row_name_;
		string input_filename_;

		bool hide_when_empty_;
		bool limit_single_child_;
		GroupWrapper group_wrapper_;
		RowWrapper row_wrapper_;

		list<SchemaElement> child_elements_;
		vector<SchemaSet*> child_sets_;

	};
};

#endif
