#ifndef datastream_reader_h
#define datastream_reader_h

#include <string>
#include <sstream>

#include <iomanip>

namespace datastream {

	using std::string;


	string read_bool(const string & name, const string & value, unsigned int precision = 0){

		// when we connect to database code
		// value will probably be repaced with handle to db cursor
		// and name might be needed to retrieve value

		return (bool)std::stoi(value)?"true":"false";
	}

	string read_int(const string & name, const string & value, unsigned int precision = 0){

		return std::to_string(std::stoi(value));
	}


	string read_double(const string & name, const string & value, unsigned int precision = 0){

		std::ostringstream out;
		out << std::setprecision(precision) << std::stof(value);
		return out.str();
	}

	string read_string(const string & name, const string & value, unsigned int precision = 0){

		return value;
	}
}

#endif
