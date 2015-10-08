#ifndef datastream_model_h
#define datastream_model_h

#include "schema.h"
#include "data.h"
#include "formatter.h"

namespace datastream {

	class Model{
	public:

		Model() = default;

		Model(const string & schema_sets_filename, const string & schema_elements_filename){
			load(schema_sets_filename, schema_elements_filename);
		}

		void load(const string & schema_sets_filename, const string & schema_elements_filename){
			schema.load(schema_sets_filename, schema_elements_filename);
			data.load(schema);
		}

		void write(ostream & os, Formatter& formatter) const{
			data.write(os, formatter);
		}

	private:

		Schema schema;
		Data data;
	};
}

#endif
