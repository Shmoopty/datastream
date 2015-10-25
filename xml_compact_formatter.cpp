#include "xml_compact_formatter.h"
namespace datastream {

	void xmlCompactFormatter::openGroup(
		ostream & os,
		const string& group_label,
		const string& row_label,
		unsigned int & siblings_written,

		RowWrapper parent_row_wrapper,

		bool single_child_per_parent,
		GroupWrapper group_wrapper

	){
		if (group_wrapper == GroupWrapper::array_wrapper){
			os << open_angle << group_label << close_angle;
		}
	};

	void xmlCompactFormatter::closeGroup(
		ostream & os,
		const string& group_label,
		const string& row_label,
		unsigned int & siblings_written,

		RowWrapper parent_row_wrapper,
		bool single_child_per_parent,
		GroupWrapper group_wrapper
	){
		if (group_wrapper == GroupWrapper::array_wrapper){
			os << open_angle << slash << group_label << close_angle;
		}
	};

	// void xmlCompactFormatter::labelChild(
	// 	ostream & os,
	// 	const string& label,
	// 	RowWrapper parent_row_wrapper,
	// 	unsigned int & siblings_written
	// ){
	// 	if (parent_row_wrapper == RowWrapper::object_wrapper){
	// 		os << Quote(label, quote) << divider;
	// 	}
	// };

	void xmlCompactFormatter::openElement(
		ostream & os,
		const string& label,
		RowWrapper row_wrapper,
		unsigned int & siblings_written
	){
		os << open_angle << label << close_angle;
	};

	void xmlCompactFormatter::closeElement(ostream & os, const string& name, RowWrapper row_wrapper, unsigned int & siblings_written )
	{
		os << open_angle << slash << name << close_angle;
	};

	void xmlCompactFormatter::writeValue(ostream & os, const string& name, const boost::optional<string>& value, ElementDataType data_type, unsigned int & siblings_written){
		if (value){
			os << *value;
		}
	};

	void xmlCompactFormatter::writeElement(
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

	void xmlCompactFormatter::openRow(ostream & os, const string& name, RowWrapper rowWrapper, unsigned int & siblings_written ){
		if (rowWrapper == RowWrapper::object_wrapper || rowWrapper == RowWrapper::array_wrapper){
			os << open_angle << name << close_angle;
		}
		else if (rowWrapper == RowWrapper::array_wrapper){
			//should name be replaced with 'value'?
			os << open_angle << name << close_angle;
		}
	}
	//closeRow(ostream & os, const string& name, RowWrapper rowWrapper){};
	void xmlCompactFormatter::closeRow(ostream & os, const string& name, RowWrapper rowWrapper)
	{
		if (rowWrapper == RowWrapper::object_wrapper){
			os << open_angle << slash << name << close_angle;
		}
		else if (rowWrapper == RowWrapper::array_wrapper){
			//should name be replaced with 'value' ?
			os << open_angle << slash << name << close_angle;
		}
	};


	void xmlCompactFormatter::writeEmptyGroup(
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

		//override?
		unsigned int no_children = 0;

		openRow(os, row_label, rowWrapper, no_children);
		closeRow(os, row_label, rowWrapper);

		if (rowWrapper != RowWrapper::no_wrapper){
			return;
		}

		writeValue(os, blank, {}, ElementDataType::type_raw, no_children);

	};
}
