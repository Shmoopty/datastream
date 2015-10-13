#ifndef datastream_model_h
#define datastream_model_h

#include "schema.h"
#include "data.h"
#include "formatter.h"

namespace datastream {

	class Model{
	public:

		// if this was adapted to produce multiple reports
		// schema can be reused
		// data currently cannot
		
		Model() = default;

		Model(const string & schema_sets_filename, const string & schema_elements_filename){
			build(schema_sets_filename, schema_elements_filename);
		}

		void build(const string & schema_sets_filename, const string & schema_elements_filename){
			schema.build(schema_sets_filename, schema_elements_filename);
			data.build(schema);
		}

		void write(ostream & os, Formatter& formatter) const{
			data.write(os, formatter, schema);
		}

	private:

		Schema schema;
		Data data;
	};
}

#endif
