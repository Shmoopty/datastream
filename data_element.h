#ifndef datastream_data_element_h
#define datastream_data_element_h

#include <string>

#include <boost/flyweight.hpp>
#include "datastream_definitions.h"

namespace datastream {

	class DataElement{
	public:

		DataElement(string value);

		DataElement();

		bool isNull() const;

		const string& getValue() const;

	private:

		boost::flyweight<string> value_ = boost::flyweight<string>{blank};
		bool is_null_ = true;
	};
}
#endif
