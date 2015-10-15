#ifndef datastream_data_h
#define datastream_data_h

#include "schema.h"
#include "schema_set.h"
#include "data_set.h"
#include "datastream_definitions.h"
#include "formatter.h"

namespace datastream {

	class Data{

	public:

		void clear();

		void build(const Schema& schema);

		void write(ostream & os, Formatter& formatter, const Schema& schema ) const;

	private:

		list<DataSet> data_set_list;
		map<int, DataSet*> data_set_ptr_map;

		void load(const list<SchemaSet>& schema_set_list);

		void map(const Schema& schema);
	};
}
#endif
