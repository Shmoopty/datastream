#include "xml_formatter.h"
namespace datastream {

	void xmlFormatter::separate(
		ostream & os,
		unsigned int siblings_written,
		unsigned int extraIndent
	){
	}

	void xmlFormatter::step(
		ostream & os,
		unsigned int siblings_written,
		unsigned int extraIndent
	){
		separate(os, siblings_written, extraIndent);
		if(!clean){
			os << new_line_token << Indent(indent,depth + extraIndent);
		}
		clean = true;
	}

	void xmlFormatter::openGroup(
		ostream & os,
		const string& group_label,
		const string& row_label,
		unsigned int & siblings_written,

		RowWrapper parent_row_wrapper,

		bool single_child_per_parent,
		GroupWrapper group_wrapper

	){
		if (group_wrapper == GroupWrapper::array_wrapper){
			step(os, siblings_written);
			os << open_angle << group_label << close_angle;
			up();
			clean = false;
		}
	};

	void xmlFormatter::closeGroup(
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
			os << open_angle << slash << group_label << close_angle;
			clean = false;
		}
	};

	void xmlFormatter::labelChild(
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

	void xmlFormatter::openElement(
		ostream & os,
		const string& label,
		RowWrapper row_wrapper,
		unsigned int & siblings_written
	){
		//if (row_wrapper == RowWrapper::object_wrapper || row_wrapper == RowWrapper::array_wrapper){
		os << open_angle << label << close_angle;
		clean = false;
		//}
	};

	void xmlFormatter::closeElement(ostream & os, const string& name, RowWrapper row_wrapper, unsigned int & siblings_written )
	{
		os << open_angle << slash << name << close_angle;
		clean = false;
	};

	void xmlFormatter::writeValue(ostream & os, const string& name, const boost::optional<string>& value, ElementDataType data_type, unsigned int & siblings_written){

		if (value){
			os << *value;
		}
		else{
			os << null_keyword;
		}
		clean = false;
	};

	void xmlFormatter::writeElement(
		ostream & os,
		const string& name,
		const boost::optional<string>& value,
		ElementDataType data_type,
		GroupWrapper parent_group_wrapper,
		RowWrapper parent_row_wrapper,
		unsigned int & siblings_written

	){
		step(os, siblings_written);
		openElement(os, name, parent_row_wrapper, siblings_written);
		writeValue(os, name, value, data_type, siblings_written);
		closeElement(os, name, parent_row_wrapper, siblings_written);
	};

	// void xmlFormatter::open(ostream & os, GroupWrapper group_wrapper){
	// };

	void xmlFormatter::openRow(ostream & os, const string& name, RowWrapper rowWrapper, unsigned int & siblings_written ){
		if (rowWrapper == RowWrapper::object_wrapper || rowWrapper == RowWrapper::array_wrapper){
			step(os);
			os << open_angle << name << close_angle;
			up();
			clean = false;
		}
		else if (rowWrapper == RowWrapper::array_wrapper){
			stepUp(os);
			//should name be replaced with 'value'?
			os << open_angle << name << close_angle;
			up();
			clean = false;
		}
	}
	//closeRow(ostream & os, const string& name, RowWrapper rowWrapper){};
	void xmlFormatter::closeRow(ostream & os, const string& name, RowWrapper rowWrapper)
	{
		if (rowWrapper == RowWrapper::object_wrapper){
			stepDown(os);
			os << open_angle << slash << name << close_angle;
			clean = false;
		}
		else if (rowWrapper == RowWrapper::array_wrapper){
			stepDown(os);
			//should name be replaced with 'value' ?
			os << open_angle << slash << name << close_angle;
			clean = false;
		}
	};


	void xmlFormatter::writeEmptyGroup(
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

		//override
		unsigned int none = 0;

		openRow(os, row_label, rowWrapper, none);
		closeRow(os, row_label, rowWrapper);

		if (rowWrapper != RowWrapper::no_wrapper){
			return;
		}

		writeValue(os, blank, {}, ElementDataType::type_raw, none);

	};


	// void xmlFormatter::close(ostream & os, GroupWrapper group_wrapper)
	// {
	// };

}
