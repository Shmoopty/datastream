#ifndef datastream_json_compact_formatter
#define datastream_json_compact_formatter


#include "formatter.h"
namespace datastream {

	class jsonCompactFormatter:public Formatter{

	public:
		virtual ~jsonCompactFormatter() = default;

		virtual void separate(
			ostream & os,
			unsigned int siblings_written = 0,
			unsigned int extraIndent = 0
		) override;

		virtual void step(
			ostream & os,
			unsigned int siblings_written = 0,
			unsigned int extraIndent = 0
		) override;

		virtual void openGroup(
			ostream & os,
			const string& group_label,
			const string& row_label,
			unsigned int & siblings_written,

			RowWrapper parent_row_wrapper,

			bool single_child_per_parent,
			GroupWrapper group_wrapper
		) override;

		virtual void closeGroup(
			ostream & os,
			const string& group_label,
			const string& row_label,
			unsigned int & siblings_written,

			RowWrapper parent_row_wrapper,
			bool single_child_per_parent,
			GroupWrapper group_wrapper
		) override;

		virtual void openElement(
			ostream & os,
			const string& label,
			RowWrapper row_wrapper,
			unsigned int & siblings_written
		) override;

		virtual void writeValue(
			ostream & os,
			const string& name,
			const boost::optional<string>& value,
			ElementDataType data_type,
			unsigned int & siblings_written
		) override;

		virtual void writeElement(
			ostream & os,
			const string& name,
			const boost::optional<string>& value,
			ElementDataType data_type,
			GroupWrapper parent_group_wrapper,
			RowWrapper parent_row_wrapper,
			unsigned int & siblings_written

		) override;

		virtual void openRow(ostream & os, const string& name, RowWrapper rowWrapper, unsigned int & siblings_written ) override;

		virtual void writeEmptyGroup(
			ostream & os,
			const string& group_label,
			const string& row_label,
			unsigned int & siblings_written,

			RowWrapper parent_row_wrapper,

			bool single_child_per_parent,
			GroupWrapper group_wrapper,
			RowWrapper rowWrapper
		) override;

		virtual void closeRow(ostream & os, const string& name, RowWrapper rowWrapper) override;
		
	private:
		virtual void labelChild(
			ostream & os,
			const string& label,
			RowWrapper parent_row_wrapper,
			unsigned int & siblings_written
		);

	};
}
#endif
