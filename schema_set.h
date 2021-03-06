#ifndef datastream_schema_set_h
#define datastream_schema_set_h

#include <string>
#include <vector>
#include <list>

#include "schema_element.h"

namespace datastream {

	using std::string;
	using std::vector;
	using std::list;

	class SchemaSet {

	public:
		SchemaSet(
			unsigned int position,

			unsigned int id,
			unsigned int parent,

		/* Drew Dormann - 
			Since std::string can be move-constructed, passing these by value
			will work with both l-values and r-values, performing minimum copies
			in both cases.  The old form would only be preferable, I believe,
			if you wanted to force calling code to perform whatever actions were
			necessary to convert their value to an r-value.   I don't think you want
			that.

			http://stackoverflow.com/questions/7592630
			https://web.archive.org/web/20140205194657/http://cpp-next.com/archive/2009/08/want-speed-pass-by-value/
		*/

			string group_name,
			string row_name,
			string filename,

			bool limit_single_child = false,
			bool hide_when_empty = false,

			GroupWrapper groupWrapper = GroupWrapper::array_wrapper,
			RowWrapper rowWrapper = RowWrapper::object_wrapper
		);

		bool hasParent() const;

		inline unsigned int id() const {return id_;};
		inline unsigned int parent() const {return parent_;}
		inline unsigned int position() const {return position_;}

		inline const string & groupName () const {return group_name_;}
		inline const string & rowName () const {return row_name_;}
		inline const string & inputFileName() const {return input_filename_;}
		inline bool hideWhenEmpty() const {return hide_when_empty_;}
		inline bool limitSingleChild() const {return limit_single_child_;}

		inline RowWrapper rowWrapper () const {return row_wrapper_;}
		inline GroupWrapper groupWrapper () const {return group_wrapper_;}
		inline const vector<const SchemaSet*>& childSets() const {return child_sets_;}
		inline const list<SchemaElement>& childElements() const {return child_elements_;}

		void connect(const SchemaSet& child_set);
		void addElement(int element_id, int element_parent, string element_name, ElementDataType element_data_type);
		bool hasChildSets() const;

	private:

		unsigned int position_;
		unsigned int id_;
		unsigned int parent_;
		string group_name_;
		string row_name_;
		string input_filename_;

		bool hide_when_empty_;
		bool limit_single_child_;
		
		GroupWrapper group_wrapper_;
		RowWrapper row_wrapper_;

		list<SchemaElement> child_elements_;
		vector<const SchemaSet*> child_sets_;
	};
};

#endif
