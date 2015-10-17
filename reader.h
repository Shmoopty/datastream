#ifndef datastream_reader_h
#define datastream_reader_h

#include <string>
#include <sstream>
#include <iomanip>

#include "datastream_definitions.h"

namespace datastream {

	using std::string;

	inline string read_bool(const string & name, const string & input, unsigned int precision = 0)
	{
		// when we connect to database code
		// input will probably be repaced with handle to db cursor
		// and name might be needed to retrieve value

		return (bool)std::stoi(input)?"true":"false";
	}

	inline string read_int(const string & name, const string & input, unsigned int precision = 0)
	{
		return std::to_string(std::stoi(input));
	}


	inline string read_double(const string & name, const string & input, unsigned int precision = 0)
	{
		std::ostringstream out;
		out << std::setprecision(precision) << std::stof(input);
		return out.str();
	}

	inline string read_string(const string & name, const string & input, unsigned int precision = 0)
	{
		return input;
	}

	typedef string (*ReaderPtr)(const string &, const string &, const unsigned int);

	const std::map<ElementDataType, ReaderPtr> reader_map = {
		{ElementDataType::type_boolean, 	&read_bool },
		{ElementDataType::type_integer, 	&read_int },
		{ElementDataType::type_double, 		&read_double },
		{ElementDataType::type_string, 		&read_string },
		{ElementDataType::type_datetime, 	&read_string },
		{ElementDataType::type_date, 		&read_string },
		{ElementDataType::type_time, 		&read_string },
		{ElementDataType::type_raw, 		&read_string }
	 };

	inline ReaderPtr getReader( const ElementDataType data_type)
	{
		return reader_map.at(data_type);
	};
}

#endif
