#ifndef datastream_json_formatter
#define datastream_json_formatter

#include "formatter.h"
namespace datastream {

	class jsonFormatter:public Formatter{

	public:

		virtual void seperate(
			ostream & os,
			unsigned int siblings_written = 0,
			unsigned int extraIndent = 0
		){
			if(siblings_written > 0){
				os << seperator;
				clean = false;
			}
		}

		virtual void step(
			ostream & os,
			unsigned int siblings_written = 0,
			unsigned int extraIndent = 0
		){
			seperate(os, siblings_written, extraIndent);
			if(!clean){
				os << new_line_token << Indent(indent,depth + extraIndent);
			}
		}

		virtual void openGroup(
			ostream & os,
			const string& group_label,
			const string& row_label,
			unsigned int & siblings_written,

			RowWrapper parent_row_wrapper,

			bool single_child_per_parent,
			GroupWrapper groupWrapper

		){
			step(os, siblings_written);

			labelElement(os, group_label, row_label, single_child_per_parent, parent_row_wrapper, siblings_written );

			if (groupWrapper == GroupWrapper::array_wrapper){
				os  << open_array_token;
				up();
			}
		};

		virtual void closeGroup(
			ostream & os,
			const string& group_label,
			const string& row_label,
			unsigned int & siblings_written,

			RowWrapper parent_row_wrapper,
			GroupWrapper groupWrapper
		){
			if (groupWrapper == GroupWrapper::array_wrapper){
				stepDown(os);
				os << close_array_token;
				clean = false;
			}
		};
		//
		// //6 parameter version in parent
		// virtual void labelElement(
		// 	ostream & os,
		// 	const string& group_label,
		// 	const string& row_label,
		// 	bool single_child_per_parent,
		//
		// 	RowWrapper parent_row_wrapper,
		// 	unsigned int & siblings_written
		//
		// ){
		// 	if (parent_row_wrapper == RowWrapper::object_wrapper){
		// 		labelElement(os,(single_child_per_parent?row_label:group_label), parent_row_wrapper, siblings_written);
		// 	}
		// };
		//
		virtual void label(
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

		virtual void openElement(
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

		virtual void writeValue(ostream & os, const string& name, const string& value, bool isNull, ElementDataType data_type, unsigned int & siblings_written){

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


		virtual void writeElement(
			ostream & os,
			const string& name,
			const string& value,
			bool isNull,
			ElementDataType data_type,
			RowWrapper parent_row_wrapper,
			unsigned int & siblings_written

		){
			step(os, siblings_written);
			label(os, name, parent_row_wrapper, siblings_written);
			writeValue(os, name, value, isNull, data_type, siblings_written);
		};


		virtual void open(ostream & os, GroupWrapper groupWrapper){
			if (groupWrapper == GroupWrapper::array_wrapper){
				os  << open_array_token;
				clean = false;
				up();
			}
		};



		virtual void openRows(ostream & os, const string& name, bool no_array_wrapper_around_group){
		};


		//RF
		virtual void openRow(ostream & os, const string& name, RowWrapper rowWrapper, unsigned int & siblings_written ){

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
				seperate(os, siblings_written);
			}

			up();

		}




		/*
		virtual void writeElement(ostream & os, const string& name, const string& value, bool isNull, ElementDataType data_type, bool first){

			if(!first){
				os  << seperator;
			}

			os
			<< new_line_token << Indent(indent,depth + 1)
			<< Quote(name, quote) << divider;

			writeValue(os, name, value, isNull, data_type, first);
		};
		*/



		virtual void closeRow(ostream & os, const string& name, RowWrapper rowWrapper){
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

		virtual void closeRows(ostream & os, const string& name, bool no_array_wrapper_around_group){

		};

		virtual void closeElement(ostream & os, const string& name, bool no_array_wrapper_around_group = false){
			down();
			if (!no_array_wrapper_around_group){
				stepDown(os);
			}
		};

		virtual void close(ostream & os, GroupWrapper groupWrapper) {
			if (groupWrapper == GroupWrapper::array_wrapper){
				os  << close_array_token;
				clean = false;
				down();
			}
		};
	protected:
		bool clean = true;
	};


}
#endif
