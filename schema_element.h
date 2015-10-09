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

		friend class DataRow;

	public:

		string read(const string & value){
			//some database stuff to retrieve value

			//then return data
			return (*reader)(name, value, precision);
		}

		SchemaElement(unsigned int id,  unsigned int parent, string&& name, ElementDataType data_type, unsigned int precision=0):
		id(id),
		parent(parent),
		name(name),
		data_type(data_type),
		precision(precision)
		{
			if(data_type == ElementDataType::type_boolean) {
				reader = &read_bool;
			}
			else if(data_type == ElementDataType::type_integer)	{
				reader = &read_int;
			}
			else if(data_type == ElementDataType::type_double)	{
				reader = &read_double;
			}
			else if(
				data_type == ElementDataType::type_string ||
				data_type == ElementDataType::type_datetime ||
				data_type == ElementDataType::type_date ||
				data_type == ElementDataType::type_time
			)	{
				reader = &read_string;
			}
			else if(data_type == ElementDataType::type_raw)	{
				reader = &read_string;
			}
		}

		int getId () const {return id;}
		int getParent () const {return parent;}
		const string& getName () const {return name;}
		ElementDataType getDataType () const {return data_type;}

	private:

		unsigned int id;
		unsigned int parent;
		string name;

		ElementDataType data_type;
		unsigned int precision;

		string (*reader)(const string &, const string &, const unsigned int);
	};
}
#endif
