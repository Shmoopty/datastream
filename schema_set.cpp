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

		/* Drew Dormann - 
			Since std::string can be move-constructed, passing these by value
			will work with both l-values and r-values, performing minimum copies
			in both cases.  The old form would only be preferable, I believe,
			only if you wanted to force calling code to perform whatever actions were
			necessary to convert their value to an r-value.   I don't think you want
			that.

			http://stackoverflow.com/questions/7592630
			https://web.archive.org/web/20140205194657/http://cpp-next.com/archive/2009/08/want-speed-pass-by-value/
 		*/

		string group_name,
		string row_name,
		string filename,

		bool limit_single_child,
		bool hide_when_empty,

		GroupWrapper groupWrapper,
		RowWrapper rowWrapper
	):
	position_(position),
	id_(id),
	parent_(parent),

	/* Drew Dormann - 
		These were getting copied in the old code, despite being passed as && */
	group_name_( std::move(group_name) ),
	row_name_( std::move(row_name) ),
	input_filename_ ( std::move(filename) ),

	hide_when_empty_ (hide_when_empty),
	limit_single_child_ (limit_single_child),

	group_wrapper_ (groupWrapper),
	row_wrapper_ (rowWrapper)
	{};

	bool SchemaSet::hasParent() const
	{
		return !( parent_ == 0 || id_ == parent_) ;
	}

	void SchemaSet::connect(const SchemaSet& child_set)
	{
		child_sets_.push_back(&child_set);
	}

	void SchemaSet::addElement(int element_id, int element_parent, string element_name, ElementDataType element_data_type)
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
