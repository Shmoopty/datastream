#ifndef datastream_schema_element_h
#define datastream_schema_element_h

#include <string>

#include "datastream_definitions.h"
#include "reader.h"

namespace datastream {

	using std::string;

	class SchemaElement {

	public:

		SchemaElement(unsigned int id,  unsigned int parent, string&& name, ElementDataType data_type, unsigned int precision=0);
		string read(const string & value) const ;

		inline unsigned int id () const {return id_;}
		inline unsigned int parent () const {return parent_;}

		inline const string& name () const {return name_;}
		inline const ElementDataType dataType () const {return data_type_;}

		inline unsigned int precision () const {return precision_;}

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
