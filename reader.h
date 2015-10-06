#ifndef datastream_reader_h
#define datastream_reader_h

#include <string>

namespace datastream {

	using std::string;

	string read_bool(const string & name, const string & value){

		// when we connect to database code
		// value will probably be repaced with handle to db cursor
		// and name might be needed to retrieve value

		if ((bool)std::stoi(value)){
			return "true";
		}
		else{
			return "false";
		}
	}

	string read_int(const string & name, const string & value){

		return std::to_string(std::stoi(value));
	}

	string read_string(const string & name, const string & value){

		return value;
	}
}

#endif
