#ifndef datastream_model_h
#define datastream_model_h

#include "schema.h"
#include "data.h"
#include "formatter.h"
#include "json_formatter.h"
#include "json_compact_formatter.h"
#include "xml_formatter.h"
#include "xml_compact_formatter.h"

namespace datastream {

	class Model{
	public:

		Model() = default;
		Model(const string & schema_sets_filename, const string & schema_elements_filename);

		void build(const string & schema_sets_filename, const string & schema_elements_filename);
		void write(ostream & os, Formatter& formatter) const;

	private:

		Schema schema;
		Data data;
	};
}

#endif
