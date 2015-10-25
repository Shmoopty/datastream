#ifndef datastream_formatter
#define datastream_formatter

#include "datastream_definitions.h"
#include "quote.h"
#include "indent.h"
#include <iostream>
#include <boost/optional.hpp>

namespace datastream {

	using std::ostream;

	class Formatter{

	public:
		virtual ~Formatter() = default;

		virtual void up(unsigned int i = 1);

		virtual void down(unsigned int i = 1);

		virtual void setDepth(unsigned int i = 1);

		/* Drew Dormann -
			This was perhaps overlooked?  I'm not certain.  I noticed it
			after implementing the "override" modifier.

			Mat
			  LabelChild is only needed internally in the json classes,
			  i've made it private
			*/

		virtual void separate(
			ostream & os,
			unsigned int siblings_written = 0,
			unsigned int extraIndent = 0
		);

		virtual void step(
			ostream & os,
			unsigned int siblings_written = 0,
			unsigned int extraIndent = 0
		);

		virtual void stepUp(
			ostream & os,
			unsigned int siblings_written = 0,
			unsigned int extraIndent = 0
		);

		virtual void stepDown(
			ostream & os,
			unsigned int siblings_written = 0,
			unsigned int extraIndent = 0
		);

		virtual void openGroup(
			ostream & os,
			const string& group_label,
			const string& row_label,
			unsigned int & siblings_written,

			RowWrapper parent_row_wrapper,
			bool single_child_per_parent,
			GroupWrapper group_wrapper
		);

		virtual void closeGroup(
			ostream & os,
			const string& group_label,
			const string& row_label,
			unsigned int & siblings_written,

			RowWrapper parent_row_wrapper,
			bool single_child_per_parent,
			GroupWrapper group_wrapper
		);

		virtual void openElement(
			ostream & os,
			const string& label,
			RowWrapper row_wrapper,
			unsigned int & siblings_written
		);

		virtual void writeValue(
			ostream & os,
			const string& name,
			const boost::optional<string>& value,
			ElementDataType data_type,
			unsigned int & siblings_written
		);

		virtual void writeElement(
			ostream & os,
			const string& name,
			const boost::optional<string>& value,
			ElementDataType data_type,
			GroupWrapper parent_group_wrapper,
			RowWrapper parent_row_wrapper,
			unsigned int & siblings_written

		);

		virtual void writeEmptyGroup(
			ostream & os,
			const string& group_label,
			const string& row_label,
			unsigned int & siblings_written,

			RowWrapper parent_row_wrapper,

			bool single_child_per_parent,
			GroupWrapper group_wrapper,
			RowWrapper rowWrapper
		);

		virtual void open(ostream & os, GroupWrapper group_wrapper);
		virtual void openRow(ostream & os, const string& name, RowWrapper rowWrapper, unsigned int & siblings_written );
		virtual void closeRow(ostream & os, const string& name, RowWrapper rowWrapper);
		virtual void closeElement(ostream & os, const string& name, RowWrapper row_wrapper, unsigned int & siblings_written );
		virtual void close(ostream & os, GroupWrapper group_wrapper);

	protected:

		string new_line_token = newline;
		string open_row_token = open_brace;
		string close_token = close_brace;

		string open_array_token = open_bracket;
		string close_array_token = close_bracket;
		string null_token = null_keyword;

		string seperator = ",";
		string divider = space_colon_space;
		string indent = tab;
		string quote = double_quote;
		int depth = 0;
		bool clean = true;

	};
}
#endif
