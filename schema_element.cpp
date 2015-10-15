#include "schema_element.h"

namespace datastream {

	using std::string;

	SchemaElement::SchemaElement(unsigned int id,  unsigned int parent, string&& name, ElementDataType data_type, unsigned int precision):
	id_(id),
	parent_(parent),
	name_(name),
	data_type_(data_type),
	precision_(precision),
	reader_(getReader(data_type))
	{}

	string SchemaElement::read(const string & value) const {
		//some database stuff to retrieve value

		//then return data
		return (*reader_)(name_, value, precision_);
	}

	int SchemaElement::id () const {return id_;}
	int SchemaElement::parent () const {return parent_;}
	const string& SchemaElement::name () const {return name_;}
	const ElementDataType SchemaElement::dataType () const {return data_type_;}
};
