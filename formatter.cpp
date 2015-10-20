#include "formatter.h"

namespace datastream {

	//Formatter::Formatter(){};

	void Formatter::up(unsigned int i ){
		depth += i;
	}

	void Formatter::down(unsigned int i ){
		depth -= i;
		if (depth < 0){
			depth=0;
		}
	}

	void Formatter::setDepth(unsigned int i ){
		depth = i;
	}

	void Formatter::separate(
		ostream & os,
		unsigned int siblings_written,
		unsigned int extraIndent
	){}

	void Formatter::step(
		ostream & os,
		unsigned int siblings_written,
		unsigned int extraIndent
	){}

	void Formatter::stepUp(
		ostream & os,
		unsigned int siblings_written,
		unsigned int extraIndent
	){
		up();
		step(os, siblings_written, extraIndent);
	}

	void Formatter::stepDown(
		ostream & os,
		unsigned int siblings_written,
		unsigned int extraIndent
	){
		down();
		step(os, siblings_written, extraIndent);
	}

	void Formatter::openGroup(
		ostream & os,
		const string& group_label,
		const string& row_label,
		unsigned int & siblings_written,

		RowWrapper parent_row_wrapper,

		bool single_child_per_parent,
		GroupWrapper group_wrapper
	){};

	void Formatter::closeGroup(
		ostream & os,
		const string& group_label,
		const string& row_label,
		unsigned int & siblings_written,

		RowWrapper parent_row_wrapper,
		bool single_child_per_parent,
		GroupWrapper group_wrapper
	){};

	void Formatter::openElement(
		ostream & os,
		const string& label,
		RowWrapper row_wrapper,
		unsigned int & siblings_written
	){};

	void Formatter::writeValue(
		ostream & os,
		const string& name,
		const string& value,
		bool isNull,
		ElementDataType data_type,
		unsigned int & siblings_written
	){};

	void Formatter::writeElement(
		ostream & os,
		const string& name,
		const string& value,
		bool isNull,
		ElementDataType data_type,
		GroupWrapper parent_group_wrapper,
		RowWrapper parent_row_wrapper,
		unsigned int & siblings_written
	){};

	void Formatter::writeEmptyGroup(
		ostream & os,
		const string& group_label,
		const string& row_label,
		unsigned int & siblings_written,

		RowWrapper parent_row_wrapper,

		bool single_child_per_parent,
		GroupWrapper group_wrapper,
		RowWrapper rowWrapper
	){};

	void Formatter::open(ostream & os, GroupWrapper group_wrapper){};

	void Formatter::openRow(ostream & os, const string& name, RowWrapper rowWrapper, unsigned int & siblings_written ){}
	void Formatter::closeRow(ostream & os, const string& name, RowWrapper rowWrapper){};
	void Formatter::closeElement(ostream & os, const string& name, RowWrapper row_wrapper, unsigned int & siblings_written ){};
	void Formatter::close(ostream & os, GroupWrapper group_wrapper){};
}
