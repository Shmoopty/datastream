#ifndef datastream_data_element_h
#define datastream_data_element_h

#include <string>

#include "datastream_definitions.h"

namespace datastream {

	class DataElement{
	public:

		DataElement(string&& value);

		DataElement();

		bool isNull() const;

		const string& getValue() const;

	private:

		string value_ = blank;
		bool is_null_ = true;
	};
}
#endif
