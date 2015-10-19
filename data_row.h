#ifndef datastream_data_row_h
#define datastream_data_row_h

#include <map>
#include <list>
#include <sstream>

#include "data_element.h"
#include "schema_set.h"
#include "datastream_definitions.h"
#include "formatter.h"

#include <boost/range/algorithm_ext/for_each.hpp>

namespace datastream {

	using std::map;
	using std::vector;
	using std::shared_ptr;
	using std::list;
	using std::ostream;

	class DataRow;

	template <typename T>
	struct is_row_containter {
	    enum { value = false };
	};

	template <>
	struct is_row_containter <std::vector<std::shared_ptr<DataRow>>> {
	    enum { value = true };
	};

	template <>
	struct is_row_containter <std::vector<std::weak_ptr<DataRow>>> {
	    enum { value = true };
	};

	template <>
	struct is_row_containter <std::vector<DataRow>> {
	    enum { value = true };
	};

	template <>
	struct is_row_containter <std::list<DataRow>> {
	    enum { value = true };
	};

	template <>
	struct is_row_containter <std::vector<DataRow*>> {
	    enum { value = true };
	};

	class DataRow{

		template <typename T>
		typename std::enable_if<is_row_containter<T>::value>::type

		friend writeRows(
			ostream & os,
			RowWrapper parent_row_wrapper_type,
			const SchemaSet& schema_set,
			T const& rows ,
			Formatter& formatter,
			unsigned int & siblings_written
		)
		{
			if(rows.size() == 0 ){
				if (schema_set.hideWhenEmpty() == false){

					formatter.writeEmptyGroup(
						os,
						schema_set.groupName(),
						schema_set.rowName(),
						siblings_written,

						parent_row_wrapper_type,

						schema_set.limitSingleChild(),
						schema_set.groupWrapper(),
						schema_set.rowWrapper()
					);
				}
				return;
			}

			formatter.openGroup(
				os,
				schema_set.groupName(),
				schema_set.rowName(),
				siblings_written,

				//parent row wrapper
				parent_row_wrapper_type, //schema_set.rowWrapper,
				schema_set.limitSingleChild(),
				schema_set.groupWrapper()
			);

			unsigned int children_written = 0;

			for (auto& row : rows){
				deref(row).write(os, schema_set, formatter, children_written);

				if(schema_set.limitSingleChild() && children_written > 0){
					break;
				}
			}

			formatter.closeGroup(
				os,
				schema_set.groupName(),
				schema_set.rowName(),
				siblings_written,

				parent_row_wrapper_type,
				schema_set.limitSingleChild(),
				schema_set.groupWrapper()
			);

			++siblings_written;
		}

	public:

		DataRow (unsigned int id, unsigned int parent, unsigned int position);

		void load(const list<SchemaElement> & schema_elements, const string & line_values);

		void connect(int set_id, std::shared_ptr<std::vector<DataRow*>> child_rows);

		void write(ostream & os, const SchemaSet& schema_set, Formatter& formatter, unsigned int & siblings_written) const;

		inline unsigned int id() const {return id_;}
		inline unsigned int parent() const {return parent_;}
		inline unsigned int position() const {return position_;}

	private:

		unsigned int id_;
		unsigned int parent_;
		unsigned int position_;
		list<DataElement> child_elements_;

		// shared with other rows in this set with the same id
		map<int, shared_ptr<vector<DataRow*>>> child_rows_by_set_id_;

	};
}
#endif
