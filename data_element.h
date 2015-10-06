#ifndef datastream_data_element_h
#define datastream_data_element_h

#include "schema_element.h"
#include "datastream_definitions.h"

namespace datastream {

	class DataElement;

	typedef list<DataElement> DataElements;

	class DataElement{
	public:

		DataElement (string&& value):value(value), is_null(false)
		{
		}

		DataElement ():value(""), is_null(true)
		{
		}

		bool isNull () const
		{
			return is_null;
		}

		const string& getValue () const
		{
			return is_null?blank:value;
		}

/* not currently used

		void setValue (string&& v)
		{
			value = v;
		}

		void setValue (const string& v)
		{
			value = v;
		}

		void setNull (bool n)
		{
			null_value = n;
			if (n){
				value = blank;
			}
		}
*/
	private:

		string value = blank;
		bool   is_null = true;
	};
}
#endif
