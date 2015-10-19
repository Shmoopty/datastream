#include "json_compact_formatter.h"
namespace datastream {

	void jsonCompactFormatter::separate(
		ostream & os,
		unsigned int siblings_written,
		unsigned int extraIndent
	){
		if(siblings_written > 0){
			os << seperator;
		}
	}

	void jsonCompactFormatter::step(
		ostream & os,
		unsigned int siblings_written,
		unsigned int extraIndent
	){
		separate(os, siblings_written, extraIndent);
	}

	void jsonCompactFormatter::openGroup(
		ostream & os,
		const string& group_label,
		const string& row_label,
		unsigned int & siblings_written,

		RowWrapper parent_row_wrapper,

		bool single_child_per_parent,
		GroupWrapper group_wrapper

	){
		separate(os, siblings_written);

		labelChild(os, group_label, parent_row_wrapper, siblings_written );

		if (group_wrapper == GroupWrapper::array_wrapper){
			os  << open_array_token;
		}
	};

	void jsonCompactFormatter::closeGroup(
		ostream & os,
		const string& group_label,
		const string& row_label,
		unsigned int & siblings_written,

		RowWrapper parent_row_wrapper,
		GroupWrapper group_wrapper
	){
		if (group_wrapper == GroupWrapper::array_wrapper){
			os << close_array_token;
		}
	};

	void jsonCompactFormatter::labelChild(
		ostream & os,
		const string& label,
		RowWrapper parent_row_wrapper,
		unsigned int & siblings_written
	){
		if (parent_row_wrapper == RowWrapper::object_wrapper){
			os << Quote(label, quote) << divider;
		}
	};

	void jsonCompactFormatter::openElement(
		ostream & os,
		const string& label,
		RowWrapper row_wrapper,
		unsigned int & siblings_written
	){
		if (row_wrapper == RowWrapper::object_wrapper){
			os << Quote(label, quote) << divider;
		}
	};

	void jsonCompactFormatter::writeValue(ostream & os, const string& name, const string& value, bool isNull, ElementDataType data_type, unsigned int & siblings_written){

		if (isNull){
			os << null_keyword;
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
			}
		}
	};


	void jsonCompactFormatter::writeElement(
		ostream & os,
		const string& name,
		const string& value,
		bool isNull,
		ElementDataType data_type,
		GroupWrapper parent_group_wrapper,
		RowWrapper parent_row_wrapper,
		unsigned int & siblings_written

	){
		separate(os, siblings_written);
		labelChild(os, name, parent_row_wrapper, siblings_written);
		writeValue(os, name, value, isNull, data_type, siblings_written);
	};


	void jsonCompactFormatter::open(ostream & os, GroupWrapper group_wrapper){
	};



	// void jsonCompactFormatter::openRows(ostream & os, const string& name, bool no_array_wrapper_around_group){
	// };


	void jsonCompactFormatter::openRow(ostream & os, const string& name, RowWrapper rowWrapper, unsigned int & siblings_written ){

		if (rowWrapper == RowWrapper::object_wrapper){
			separate(os, siblings_written);
			os  << open_row_token;
		}
		else if (rowWrapper == RowWrapper::array_wrapper){
			separate(os, siblings_written);
			os  << open_array_token;
		}
		else if (rowWrapper == RowWrapper::no_wrapper){
			separate(os, siblings_written);
		}
	}

	void jsonCompactFormatter::writeEmptyGroup(
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
			group_wrapper
		);

		if (group_wrapper == GroupWrapper::array_wrapper){
			return;
		}

		//override
		unsigned int none = 0;

		openRow(os, row_label, rowWrapper, none);
		closeRow(os, row_label, rowWrapper);

		if (rowWrapper != RowWrapper::no_wrapper){
			return;
		}

		writeValue(os, blank, blank, true, ElementDataType::type_raw, none);

	};

	void jsonCompactFormatter::closeRow(ostream & os, const string& name, RowWrapper rowWrapper)
	{
		if (rowWrapper == RowWrapper::object_wrapper){
			separate(os);
			os  << close_token;
		}
		else if (rowWrapper == RowWrapper::array_wrapper){
			separate(os);
			os  << close_array_token;
		}
		// else if (rowWrapper == RowWrapper::no_wrapper){
		// }
	};

	// void jsonCompactFormatter::closeRows(ostream & os, const string& name, bool no_array_wrapper_around_group)
	// {
	//
	// };

	void jsonCompactFormatter::closeElement(ostream & os, const string& name, RowWrapper row_wrapper, unsigned int & siblings_written )
	{
		if (row_wrapper != RowWrapper::no_wrapper){
			separate(os);
		}
	};

	void jsonCompactFormatter::close(ostream & os, GroupWrapper group_wrapper)
	{
	};

}
