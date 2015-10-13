#ifndef datastream_schema_element_h
#define datastream_schema_element_h

#include <string>
#include <vector>
#include <list>

#include "datastream_definitions.h"
#include "reader.h"

namespace datastream {

	using std::string;

	class SchemaElement {

	public:

		SchemaElement(unsigned int id,  unsigned int parent, string&& name, ElementDataType data_type, unsigned int precision=0):
		id_(id),
		parent_(parent),
		name_(name),
		data_type_(data_type),
		precision_(precision)
		{
			if(data_type_ == ElementDataType::type_boolean) {
				reader_ = &read_bool;
			}
			else if(data_type_ == ElementDataType::type_integer)	{
				reader_ = &read_int;
			}
			else if(data_type_ == ElementDataType::type_double)	{
				reader_ = &read_double;
			}
			else if(
				data_type_ == ElementDataType::type_string ||
				data_type_ == ElementDataType::type_datetime ||
				data_type_ == ElementDataType::type_date ||
				data_type_ == ElementDataType::type_time
			)	{
				reader_ = &read_string;
			}
			else if(data_type_ == ElementDataType::type_raw)	{
				reader_ = &read_string;
			}
		}

		string read(const string & value) const {
			//some database stuff to retrieve value

			//then return data
			return (*reader_)(name_, value, precision_);
		}

		// if usae of this class is limited to friend classes
		// maybe this is a redundant mix of patterns

		int id () const {return id_;}
		int parent () const {return parent_;}
		const string& name () const {return name_;}
		ElementDataType dataType () const {return data_type_;}

	private:

		unsigned int id_;
		unsigned int parent_;
		string name_;

		ElementDataType data_type_;
		unsigned int precision_;

		string (*reader_)(const string &, const string &, const unsigned int);
	};
}
#endif
