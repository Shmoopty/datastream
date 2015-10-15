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

		SchemaElement(unsigned int id,  unsigned int parent, string&& name, ElementDataType data_type, unsigned int precision=0);
		string read(const string & value) const ;

		int id () const ; //{return id_;}
		int parent () const ; //{return parent_;}
		const string& name () const ;//const {return name_;}
		const ElementDataType dataType () const;// {return data_type_;}

	private:

		const unsigned int id_;
		const unsigned int parent_;
		const string name_;

		const ElementDataType data_type_;
		const unsigned int precision_;

		const ReaderPtr reader_;

	};
}
#endif
