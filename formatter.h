#ifndef datastream_formatter
#define datastream_formatter

#include "datastream_definitions.h"

#include <iostream>

namespace datastream {

	using std::ostream;

	class Formatter;

	class Quote{
	public:
		friend ostream& operator<<(std::ostream& os, const Quote& obj){
			obj.write(os);
		  return os;
		}
		Quote() = delete;
		Quote(const string & text, const string & quote):
		text(text), quote(quote){}

		void write(ostream & os) const{
			os << quote << text << quote;
		}

	private:
		const string & text;
		const string & quote;
	};

	class Indent{
	public:

		friend ostream& operator<<(std::ostream& os, const Indent& obj){
			obj.write(os);
		  return os;
		}

		Indent() = delete;
		Indent(const string & indent, unsigned int depth = 1):
			indent(indent),
			depth(depth){}

		void write(ostream & os) const{
			for (int i=0; i < depth; ++i){
				os << indent;
			}
		}

	private:
		const string & indent;
		unsigned int depth;
	};

	class Formatter{

	public:

		virtual void up(unsigned int i = 1){
			depth += i;
		}

		virtual void down(unsigned int i = 1){
			depth -= i;
			if (depth < 0){
				depth=0;
			}
		}

		virtual void setDepth(unsigned int i = 1){
			depth = i;
		}

		virtual void seperate(
			ostream & os,
			unsigned int siblings_written = 0,
			unsigned int extraIndent = 0
		){}

		virtual void step(
			ostream & os,
			unsigned int siblings_written = 0,
			unsigned int extraIndent = 0
		){}

		virtual void stepUp(
			ostream & os,
			unsigned int siblings_written = 0,
			unsigned int extraIndent = 0
		){
			up();
			step(os, siblings_written, extraIndent);
		}

		virtual void stepDown(
			ostream & os,
			unsigned int siblings_written = 0,
			unsigned int extraIndent = 0
		){
			down();
			step(os, siblings_written, extraIndent);
		}

		virtual void openGroup(
			ostream & os,
			const string& group_label,
			const string& row_label,
			unsigned int & siblings_written,

			RowWrapper parent_row_wrapper,

			bool single_child_per_parent,
			GroupWrapper groupWrapper
		){};

		virtual void closeGroup(
			ostream & os,
			const string& group_label,
			const string& row_label,
			unsigned int & siblings_written,

			RowWrapper parent_row_wrapper,
			GroupWrapper groupWrapper
		){};

		virtual void labelElement(
			ostream & os,
			const string& group_label,
			const string& row_label,
			bool single_child_per_parent,

			RowWrapper parent_row_wrapper,
			unsigned int & siblings_written

		){
			if (parent_row_wrapper == RowWrapper::object_wrapper){
				label(os,(single_child_per_parent?row_label:group_label), parent_row_wrapper, siblings_written);
			}
		};

		virtual void label(
			ostream & os,
			const string& label,
			RowWrapper parent_row_wrapper,
			unsigned int & siblings_written
		){};

		// {
		// 	if (parent_row_wrapper == RowWrapper::object_wrapper){
		// 		os << Quote(label, quote) << divider;
		// 	}
		// };

		virtual void openElement(
			ostream & os,
			const string& label,
			RowWrapper row_wrapper,
			unsigned int & siblings_written
		){};

		virtual void writeValue(ostream & os, const string& name, const string& value, bool isNull, ElementDataType data_type, unsigned int & siblings_written){};

		virtual void writeElement(
			ostream & os,
			const string& name,
			const string& value,
			bool isNull,
			ElementDataType data_type,
			RowWrapper parent_row_wrapper,
			unsigned int & siblings_written

		){};

		virtual void open(ostream & os, GroupWrapper groupWrapper){};
		virtual void openRows(ostream & os, const string& name, bool no_array_wrapper_around_group){};
		virtual void openRow(ostream & os, const string& name, RowWrapper rowWrapper, unsigned int & siblings_written ){}
		virtual void closeRow(ostream & os, const string& name, RowWrapper rowWrapper){};
		virtual void closeRows(ostream & os, const string& name, bool no_array_wrapper_around_group){};
		virtual void closeElement(ostream & os, const string& name, bool no_array_wrapper_around_group = false){};
		virtual void close(ostream & os, GroupWrapper groupWrapper){};

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
	};
}
#endif
