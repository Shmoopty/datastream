#include "data_element.h"

namespace datastream {

	DataElement::DataElement(string value):
	/* Drew Dormann -
		move would be apprpriate here regardless of whether it's passed 
		as string or string&& */
	value_{std::move(value)}
	{}

	DataElement::DataElement():
	value_{}
	{}

	const boost::optional<string>& DataElement::getValue() const
	{
		return value_;
	}
}
