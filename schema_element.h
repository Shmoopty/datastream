#ifndef datastream_schema_element_h
#define datastream_schema_element_h

#include <string>
#include <vector>

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
			return (*reader)(name, value);
		}


		SchemaElement(unsigned int id,  unsigned int parent, string&& name, ElementDatatype datatype, unsigned int precision=0):
		id(id),
		parent(parent),
		name(name),
		datatype(datatype),
		precision(precision)
		{
			if(datatype == ElementDatatype::type_boolean) {
				reader = &read_bool;
			}
			else if(datatype == ElementDatatype::type_integer)	{
				reader = &read_int;
			}
			else if(datatype == ElementDatatype::type_double)	{
				reader = &read_int;
			}
			else if(
				datatype == ElementDatatype::type_string ||
				datatype == ElementDatatype::type_datetime ||
				datatype == ElementDatatype::type_date ||
				datatype == ElementDatatype::type_time
			)	{
				reader = &read_string;
			}
			else if(datatype == ElementDatatype::type_raw)	{
				reader = &read_string;
			}
		}

		int getId () const {return id;}
		int getParent () const {return parent;}
		string getName () const {return name;}
		ElementDatatype getDatatype () const {return datatype;}

	private:

		//unsigned int sort;
		unsigned int id;
		unsigned int parent;
		string name;
		ElementDatatype datatype;

		unsigned int precision;

		string (*reader)(const string &, const string &);


	};
}
#endif
