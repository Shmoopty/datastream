#include "data_element.h"

namespace datastream {

	DataElement::DataElement(string value):
	/* Drew Dormann -
		move would be apprpriate here regardless of whether it's passed 
		as string or string&& */
	value_(std::move(value)),
	is_null_(false)
	{}

	DataElement::DataElement():
	value_(blank),
	is_null_(true)
	{}

	bool DataElement::isNull() const
	{
		return is_null_;
	}

	const string& DataElement::getValue() const
	{
		return is_null_?blank:value_;
	}
}
