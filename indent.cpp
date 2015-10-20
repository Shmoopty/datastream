#include "indent.h"

namespace datastream {

	using std::ostream;

	class Formatter;

	Indent::Indent(const string & indent, unsigned int depth):
		indent(indent),
		depth(depth){}

	void Indent::write(ostream & os) const{
		for (int i=0; i < depth; ++i){
			os << indent;
		}
	}
}
