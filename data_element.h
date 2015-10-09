#ifndef datastream_data_element_h
#define datastream_data_element_h

#include "datastream_definitions.h"

namespace datastream {

	class DataElement{
	public:

		DataElement(string&& value):
		value(value),
		is_null(false)
		{}

		DataElement():
		value(blank),
		is_null(true)
		{}

		bool isNull() const
		{
			return is_null;
		}

		const string& getValue() const
		{
			return is_null?blank:value;
		}

	private:

		string value = blank;
		bool   is_null = true;
	};
}
#endif
