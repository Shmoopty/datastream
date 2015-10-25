#ifndef datastream_data_element_h
#define datastream_data_element_h

#include <string>

#include <boost/flyweight.hpp>
#include <boost/optional.hpp>
#include "datastream_definitions.h"

namespace datastream {

	class DataElement{
	public:

		DataElement(string value);

		DataElement();

		using StringType = boost::flyweight<string>;
		const boost::optional<string>& getValue() const;

	private:

		/* Drew Dormann -
			A default-constructed optional is always "no value" */
		boost::optional<string> value_ = {};
	};
}
#endif
