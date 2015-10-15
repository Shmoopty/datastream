#include "schema_set.h"

namespace datastream {

	using std::string;
	using std::vector;
	using std::list;
	using std::map;

	SchemaSet::SchemaSet(
		unsigned int position,
		
		unsigned int id,
		unsigned int parent,

		string&& group_name,
		string&& row_name,
		string&& filename,

		bool limit_single_child,
		bool hide_when_empty,

		GroupWrapper groupWrapper,
		RowWrapper rowWrapper
	):
	position_(position),
	id_(id),
	parent_(parent),

	group_name_(group_name),
	row_name_(row_name),
	input_filename_ (filename),

	hide_when_empty_ (hide_when_empty),
	limit_single_child_ (limit_single_child),

	group_wrapper_ (groupWrapper),
	row_wrapper_ (rowWrapper)
	{};

	bool SchemaSet::hasParent() const
	{
		return !( parent_ == 0 || id_ == parent_) ;
	}

	void SchemaSet::connect(SchemaSet& child_set)
	{
		child_sets_.push_back(&child_set);
	}

	void SchemaSet::addElement(int element_id, int element_parent, string&& element_name, ElementDataType element_data_type)
	{
		child_elements_.emplace_back(
			element_id,
			element_parent,
			std::move(element_name),
			element_data_type
		);
	}

	bool SchemaSet::hasChildSets() const
	{
		return child_sets_.size() > 0;
	}

};
