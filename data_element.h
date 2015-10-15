#ifndef datastream_data_element_h
#define datastream_data_element_h

#include "datastream_definitions.h"

namespace datastream {

	class DataElement{
	public:

		DataElement(string&& value):
		value_(value),
		is_null_(false)
		{}

		DataElement():
		value_(blank),
		is_null_(true)
		{}

		bool isNull() const
		{
			return is_null_;
		}

		const string& getValue() const
		{
			return is_null_?blank:value_;
		}

	private:
		
		string value_ = blank;
		bool   is_null_ = true;
	};
}
#endif
