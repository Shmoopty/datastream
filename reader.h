#ifndef datastream_reader_h
#define datastream_reader_h

#include <string>
#include <sstream>
#include <iomanip>

#include "datastream_definitions.h"

namespace datastream {

	using std::string;

	inline string read_bool(const string & name, const string & value, unsigned int precision = 0){

		// when we connect to database code
		// value will probably be repaced with handle to db cursor
		// and name might be needed to retrieve value

		return (bool)std::stoi(value)?"true":"false";
	}

	inline string read_int(const string & name, const string & value, unsigned int precision = 0){

		return std::to_string(std::stoi(value));
	}


	inline string read_double(const string & name, const string & value, unsigned int precision = 0){

		std::ostringstream out;
		out << std::setprecision(precision) << std::stof(value);
		return out.str();
	}

	inline string read_string(const string & name, const string & value, unsigned int precision = 0){

		return value;
	}

	typedef string (*ReaderPtr)(const string &, const string &, const unsigned int);

	inline ReaderPtr getReader( const ElementDataType data_type_){
		if(data_type_ == ElementDataType::type_boolean){
			return &read_bool;
		}
		else if(data_type_ == ElementDataType::type_integer){
			return &read_int;
		}
		else if(data_type_ == ElementDataType::type_double){
			return &read_double;
		}
		else if(
			data_type_ == ElementDataType::type_string ||
			data_type_ == ElementDataType::type_datetime ||
			data_type_ == ElementDataType::type_date ||
			data_type_ == ElementDataType::type_time
		){
			return &read_string;
		}

		return &read_string;

	};
}

#endif
