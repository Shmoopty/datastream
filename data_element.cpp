#include "data_element.h"

namespace datastream {

	DataElement::DataElement(string&& value):
	value_(value),
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
