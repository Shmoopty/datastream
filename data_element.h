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

		/* Drew Dormann -
			I'm preserving the interface when convenient, but I
			suggest a change here because there is a strong relationship 
			between isNull() and getValue() that the interface doesn't
			communicate - that the value returned from getValue might not
			be "the truth" 
		 */
				
		// bool isNull() const;

		// const string& getValue() const;

		using StringType = boost::flyweight<string>;
		const boost::optional<string>& getValue() const;

	private:

		/* Drew Dormann -
			A default-constructed optional is always "no value" */
		boost::optional<string> value_ = {};
	};
}
#endif
