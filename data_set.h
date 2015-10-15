#ifndef datastream_data_set_h
#define datastream_data_set_h

#include <list>
#include <map>
#include "datastream_definitions.h"
#include "data_row.h"
#include "schema_set.h"
#include "formatter.h"

namespace datastream {

	using std::map;
	using namespace datastream;

	class DataSet{
	public:

		DataSet(const SchemaSet& schema_set);

		void load(const list<SchemaElement> & schema_elements, const string & line);

		// rows are stored in a list to give us a stable address
		// now put pointers to rows into vector
		// for sorting and random access

		void mapRows(DataSet* parent_set_ptr = nullptr);

		void write (ostream & os, Formatter& formatter) const;

		inline bool hasParent() const {
			return schema_set.hasParent();
		}

		inline unsigned int parent() const {
			return schema_set.parent();
		}

		inline unsigned int id() const {
			return schema_set.id();
		}

		private:
			const SchemaSet& schema_set;
			list<DataRow> rows;
			map<int, vector<DataRow*>> id_to_rows_map;

	};
}
#endif
