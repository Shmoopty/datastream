#ifndef datastream_definitions_h
#define datastream_definitions_h

// these definitions are needed in places spread throughout the namespace,
// and could be maintained in more local files, but for ease of maintainence
// they are all here

#include <string>
#include <regex>
#include <map>

namespace datastream {

	using std::string;
	using std::map;
	using std::regex;

	const string tab = "	";
	const string newline = "\n";
	const string blank = "";
	const string comma = ",";
	const string double_quote = "\"";
	const string open_brace = "{";
	const string close_brace = "}";
	const string open_bracket = "[";
	const string close_bracket = "]";
	const string null_keyword = "null";
	const string space_colon_space = " : ";

	//element data types

	enum class ElementDatatype {
		type_boolean,
		type_integer,
		type_double,
		type_string,
		type_datetime,
		type_date,
		type_time,
		type_raw
	};

	enum class GroupWrapper {
		array_wrapper,
		no_wrapper
	};

	enum class RowWrapper{
		object_wrapper,
		array_wrapper,
		no_wrapper
	};

	//look up maps
	std::map<string, ElementDatatype> element_datatype_map = {
		{"bool", 		ElementDatatype::type_boolean },
		{"boolean", 	ElementDatatype::type_boolean },

		{"int", 		ElementDatatype::type_integer },
		{"integer", 	ElementDatatype::type_integer },

		{"decimal", 	ElementDatatype::type_double  },
		{"double", 		ElementDatatype::type_double  },

		{"string", 		ElementDatatype::type_string  },

		{"datetime", 	ElementDatatype::type_datetime},
		{"date", 		ElementDatatype::type_date    },
		{"time", 		ElementDatatype::type_time    },
		{"raw", 		ElementDatatype::type_raw     }    //raw string, do not quote
	 };

	 std::map<string, GroupWrapper> group_wrapper_map = {
  		{"array", 		GroupWrapper::array_wrapper },
  		{"none", 		GroupWrapper::no_wrapper }
  	 };

 	std::map<string, RowWrapper> row_wrapper_map = {
 		{"object", 		RowWrapper::object_wrapper },
 		{"array", 		RowWrapper::array_wrapper },
 		{"none", 		RowWrapper::no_wrapper }
 	 };

	const string rx_start   	= "^";

	const string rx_comma 	    = "(?:,)";
	const string rx_bit 		= "([01])";
	const string rx_int         = "([0-9]{1,8})";
	const string rx_any         = "(.*)";
	const string rx_datatype 	= "(bool|boolean|int|integer|decimal|double|string|datetime|date|time|raw)";
	const string rx_group_wrap	= "(array|none)";
	const string rx_row_wrap	= "(object|array|none)";

	const string rx_comma_any_optional = "(" + rx_comma + rx_any + ")?";

	const string rx_whitespace	= "(\\s*)";
	const string rx_hash 	    = "(#)";

	const string rx_end   		= "$";

	const string rx_comma_bit 	= rx_comma + rx_bit;
	const string rx_comma_int   = rx_comma + rx_int;
	const string rx_comma_any   = rx_comma + rx_any;
	const string rx_comma_datatype  = rx_comma + rx_datatype;
	const string rx_comma_row_wrap = rx_comma + rx_row_wrap;
	const string rx_comma_group_wrap = rx_comma + rx_group_wrap;

	auto blank_pattern   = regex(rx_start + rx_whitespace + rx_end);
	auto comment_pattern = regex(rx_start + rx_hash + rx_any);

	const unsigned int match_index_schema_id = 1;
	const unsigned int match_index_schema_parent = 2;
	const unsigned int match_index_group_name  = 3;
	const unsigned int match_index_row_name  = 4;
	const unsigned int match_index_input_filename = 5;
	const unsigned int match_index_is_root = 6;
	const unsigned int match_index_hide_when_empty = 7;
	const unsigned int match_force_single_row_per_parent = 8;
	const unsigned int match_group_wrapper_type = 9;
	const unsigned int match_row_wrapper_type = 10;

	const unsigned int match_index_element_id = 1;
	const unsigned int match_index_element_set = 2;
	const unsigned int match_index_element_name = 3;
	const unsigned int match_index_element_datatype = 4;

	const unsigned int match_index_data_row_id = 1;
	const unsigned int match_index_data_parent = 2;
	const unsigned int match_index_data_element_values = 4;

	auto schema_set_line_pattern = regex
		(
			//start
			rx_start +

			// id
			rx_int+

			// parent,
			rx_comma_int +

			// group name,
			rx_comma_any +

			// row name,
			rx_comma_any +

			// filename,
			rx_comma_any +

			// is root,
			rx_comma_bit +

			// hide_when_empty,
			rx_comma_bit +

			// limit_single_child,
			rx_comma_bit +

			// wrapper_around_group,
			rx_comma_group_wrap +

			// wrapper_around_row
			rx_comma_row_wrap +

			// end
			rx_end
		);

		auto schema_element_line_pattern = regex(
			//start
			rx_start +

			// id,
			rx_int +

			// parent,
			rx_comma_int +

			// name,
			rx_comma_any +

			// datatype,
			rx_comma + rx_datatype +

			// end
			rx_end
		);

		//auto data_row_line_pattern = std::regex("^([0-9]{1,3})(?:,)([0-9]{1,3})(?:,{0,1})(.*)$");
		//refactored
		//^([0-9]{1,3})((?:,{1})(.+))*$
		auto data_row_line_pattern = regex(
			rx_start +
			//row
			rx_int +
			// parent,
			rx_comma_int +
			//values
			rx_comma_any_optional +
			rx_end
		);

		auto comma_pattern = regex(rx_comma);

		bool isBlank (const std::string & line){
			return std::regex_match (line,blank_pattern);
		}

		bool isComment (const std::string & line){
			return std::regex_match(line,comment_pattern);
		}

		typedef std::sregex_token_iterator pattern_iterator;
}

#endif
