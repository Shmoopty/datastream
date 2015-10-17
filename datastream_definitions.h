#ifndef datastream_definitions_h
#define datastream_definitions_h

// these definitions are needed in places spread throughout the namespace,
// and could be maintained in more local files, but for ease of maintainence
// they are all here

#include <string>
#include <regex>
#include <map>
#include <type_traits>
#include <memory>
#include <functional>

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

	enum class ElementDataType {
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

	enum class Format{
		json,
		xml
	};

	const std::map<char, Format> format_map = {
		{'x',    Format::xml },
		{'j', 	 Format::json }
	};


	enum class Style{
		pretty,
		compact
	};

   const std::map<char, Style> style_map = {
	   {'p', 		Style::pretty },
	   {'c', 		Style::compact }
	};

	enum class Structure{
		tree,
		flat,
		denormalised
	};

	const std::map<char, Structure> strucure_map = {
 	   {'t', 		Structure::tree },
 	   {'f', 		Structure::flat },
	   {'d', 		Structure::denormalised },
 	};


	//look up maps
	const std::map<string, ElementDataType> element_data_type_map = {
		{"bool", 		ElementDataType::type_boolean },
		{"boolean", 	ElementDataType::type_boolean },

		{"int", 		ElementDataType::type_integer },
		{"integer", 	ElementDataType::type_integer },

		{"decimal", 	ElementDataType::type_double  },
		{"double", 		ElementDataType::type_double  },

		{"string", 		ElementDataType::type_string  },

		{"datetime", 	ElementDataType::type_datetime},
		{"date", 		ElementDataType::type_date    },
		{"time", 		ElementDataType::type_time    },
		{"raw", 		ElementDataType::type_raw     }    //raw string, do not quote
	 };

	 const std::map<string, GroupWrapper> group_wrapper_map = {
		{"array", 		GroupWrapper::array_wrapper },
		{"none", 		GroupWrapper::no_wrapper }
	 };

	const std::map<string, RowWrapper> row_wrapper_map = {
		{"object", 		RowWrapper::object_wrapper },
		{"array", 		RowWrapper::array_wrapper },
		{"none", 		RowWrapper::no_wrapper }
	 };

	const string rx_start   	= "^";

	const string rx_comma 	    = "(?:,)";
	const string rx_bit 		= "([01])";
	const string rx_int         = "([0-9]{1,8})";
	const string rx_any         = "(.*)";
	const string rx_data_type 	= "(bool|boolean|int|integer|decimal|double|string|datetime|date|time|raw)";
	const string rx_group_wrap	= "(array|none)";
	const string rx_row_wrap	= "(object|array|none)";

	const string rx_comma_any_optional = "(" + rx_comma + rx_any + ")?";

	const string rx_whitespace	= "(\\s*)";
	const string rx_hash 	    = "(#)";

	const string rx_end   		= "$";

	const string rx_comma_bit 	= rx_comma + rx_bit;
	const string rx_comma_int   = rx_comma + rx_int;
	const string rx_comma_any   = rx_comma + rx_any;
	const string rx_comma_data_type  = rx_comma + rx_data_type;
	const string rx_comma_row_wrap = rx_comma + rx_row_wrap;
	const string rx_comma_group_wrap = rx_comma + rx_group_wrap;

	const regex blank_pattern   = regex(rx_start + rx_whitespace + rx_end);
	const regex comment_pattern = regex(rx_start + rx_hash + rx_any);

	const unsigned int match_index_schema_id = 1;
	const unsigned int match_index_schema_parent = 2;
	const unsigned int match_index_group_name  = 3;
	const unsigned int match_index_row_name  = 4;
	const unsigned int match_index_input_filename = 5;
	const unsigned int match_index_hide_when_empty = 6;
	const unsigned int match_force_single_row_per_parent = 7;
	const unsigned int match_group_wrapper_type = 8;
	const unsigned int match_row_wrapper_type = 9;

	const unsigned int match_index_element_id = 1;
	const unsigned int match_index_element_set = 2;
	const unsigned int match_index_element_name = 3;
	const unsigned int match_index_element_data_type = 4;

	const unsigned int match_index_data_row_id = 1;
	const unsigned int match_index_data_parent = 2;
	const unsigned int match_index_data_element_values = 4;

	const regex schema_set_line_pattern = regex
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

		// // is root,
		// rx_comma_bit +

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

	const regex schema_element_line_pattern = regex(
		//start
		rx_start +

		// id,
		rx_int +

		// parent,
		rx_comma_int +

		// name,
		rx_comma_any +

		// data_type,
		rx_comma + rx_data_type +

		// end
		rx_end
	);

	const regex data_row_line_pattern = regex(
		rx_start +
		//row
		rx_int +
		// parent,
		rx_comma_int +
		//values
		rx_comma_any_optional +
		rx_end
	);

	const regex comma_pattern = regex(rx_comma);

	inline bool isBlank (const std::string & line){
		return std::regex_match (line,blank_pattern);
	}

	inline bool isComment (const std::string & line){
		return std::regex_match(line,comment_pattern);
	}

	typedef std::sregex_token_iterator pattern_iterator;

	template<typename T>
	T& deref(T &v) {
	  return v;
	}

	template<typename T>
	const T& deref(const T& v) {
	  return v;
	}

	template<typename T>
	typename std::enable_if<!std::is_function<T>::value, T&>::type deref(T* v) {
	  return deref(*v);
	}

	template<typename T>
	const T& deref(const std::shared_ptr<T>& v) {
	  return deref(*v);
	}

	template<typename T>
	const T& deref(const std::weak_ptr<T>& v) {
	  return deref(*v);
	}

}

#endif
