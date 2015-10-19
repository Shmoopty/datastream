#include "json_formatter.h"
namespace datastream {

	void jsonFormatter::separate(
		ostream & os,
		unsigned int siblings_written,
		unsigned int extraIndent
	){
		if(siblings_written > 0){
			os << seperator;
			clean = false;
		}
	}

	void jsonFormatter::step(
		ostream & os,
		unsigned int siblings_written,
		unsigned int extraIndent
	){
		separate(os, siblings_written, extraIndent);
		if(!clean){
			os << new_line_token << Indent(indent,depth + extraIndent);
		}
	}

	void jsonFormatter::openGroup(
		ostream & os,
		const string& group_label,
		const string& row_label,
		unsigned int & siblings_written,

		RowWrapper parent_row_wrapper,

		bool single_child_per_parent,
		GroupWrapper group_wrapper

	){
		step(os, siblings_written);

		labelChild(os, group_label, parent_row_wrapper, siblings_written );

		if (group_wrapper == GroupWrapper::array_wrapper){
			os  << open_array_token;
			up();
		}
	};

	void jsonFormatter::closeGroup(
		ostream & os,
		const string& group_label,
		const string& row_label,
		unsigned int & siblings_written,

		RowWrapper parent_row_wrapper,
		bool single_child_per_parent,
		GroupWrapper group_wrapper
	){
		if (group_wrapper == GroupWrapper::array_wrapper){
			stepDown(os);
			os << close_array_token;
			clean = false;
		}
	};

	void jsonFormatter::labelChild(
		ostream & os,
		const string& label,
		RowWrapper parent_row_wrapper,
		unsigned int & siblings_written
	){
		if (parent_row_wrapper == RowWrapper::object_wrapper){
			os << Quote(label, quote) << divider;
			clean = false;
		}
	};

	void jsonFormatter::openElement(
		ostream & os,
		const string& label,
		RowWrapper row_wrapper,
		unsigned int & siblings_written
	){
		if (row_wrapper == RowWrapper::object_wrapper){
			os << Quote(label, quote) << divider;
			clean = false;
		}
	};

	void jsonFormatter::writeValue(ostream & os, const string& name, const string& value, bool isNull, ElementDataType data_type, unsigned int & siblings_written){

		if (isNull){
			os << null_keyword;
			clean = false;
		}
		else{
			if (
				data_type == ElementDataType::type_string ||
				data_type == ElementDataType::type_datetime ||
				data_type == ElementDataType::type_date ||
				data_type == ElementDataType::type_time
			){
				os << Quote(value, quote);
			}
			else{
				os << value;
				clean = false;
			}
		}
	};

	void jsonFormatter::writeElement(
		ostream & os,
		const string& name,
		const string& value,
		bool isNull,
		ElementDataType data_type,
		GroupWrapper parent_group_wrapper,
		RowWrapper parent_row_wrapper,
		unsigned int & siblings_written

	){
		if (!
			(
				parent_row_wrapper == RowWrapper::no_wrapper &&
				parent_group_wrapper == GroupWrapper::no_wrapper
			)
		){
			step(os, siblings_written);
		}
		labelChild(os, name, parent_row_wrapper, siblings_written);
		writeValue(os, name, value, isNull, data_type, siblings_written);
	};

	void jsonFormatter::openRow(ostream & os, const string& name, RowWrapper rowWrapper, unsigned int & siblings_written ){

		if (rowWrapper == RowWrapper::object_wrapper){
			step(os, siblings_written);
			os  << open_row_token;
			clean = false;
		}
		else if (rowWrapper == RowWrapper::array_wrapper){
			step(os, siblings_written);
			os  << open_array_token;
			clean = false;
		}
		else if (rowWrapper == RowWrapper::no_wrapper){
			separate(os, siblings_written);
		}
		up();
	}

	void jsonFormatter::writeEmptyGroup(
		ostream & os,
		const string& group_label,
		const string& row_label,
		unsigned int & siblings_written,

		RowWrapper parent_row_wrapper,

		bool single_child_per_parent,
		GroupWrapper group_wrapper,
		RowWrapper rowWrapper
	){

		openGroup(
			os,
			group_label,
			row_label,
			siblings_written,

			parent_row_wrapper,

			single_child_per_parent,
			group_wrapper
		);

		closeGroup(
			os,
			group_label,
			row_label,
			siblings_written,

			parent_row_wrapper,
			single_child_per_parent,
			group_wrapper
		);

		if (group_wrapper == GroupWrapper::array_wrapper){
			return;
		}

		unsigned int no_children = 0;

		openRow(os, row_label, rowWrapper, no_children);
		closeRow(os, row_label, rowWrapper);

		if (rowWrapper != RowWrapper::no_wrapper){
			return;
		}
		writeValue(os, blank, blank, true, ElementDataType::type_raw, no_children);
	};

	void jsonFormatter::closeRow(ostream & os, const string& name, RowWrapper rowWrapper)
	{
		if (rowWrapper == RowWrapper::object_wrapper){
			stepDown(os);
			os  << close_token;
			clean = false;
		}
		else if (rowWrapper == RowWrapper::array_wrapper){
			stepDown(os);
			os  << close_array_token;
			clean = false;
		}
		else if (rowWrapper == RowWrapper::no_wrapper){
			down();
		}
	};

	void jsonFormatter::closeElement(ostream & os, const string& name, RowWrapper row_wrapper, unsigned int & siblings_written )
	{
		stepDown(os);
	};
}
