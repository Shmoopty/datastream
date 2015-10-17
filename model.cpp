#include "model.h"

namespace datastream {

	Model::Model(const string & schema_sets_filename, const string & schema_elements_filename)
	{
		build(schema_sets_filename, schema_elements_filename);
	}

	void Model::build(const string & schema_sets_filename, const string & schema_elements_filename)
	{
		schema.build(schema_sets_filename, schema_elements_filename);
		data.build(schema);
	}

	void Model::write(ostream & os, Formatter& formatter) const
	{
		data.write(os, formatter, schema);
	}

}
