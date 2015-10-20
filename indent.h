#ifndef datastream_formatter_indent
#define datastream_formatter_indent

#include "datastream_definitions.h"

#include <iostream>

namespace datastream {

	using std::ostream;

	class Formatter;

	class Indent{
	public:

		friend ostream& operator<<(std::ostream& os, const Indent& obj){
			obj.write(os);
		  return os;
		}

		Indent() = delete;
		Indent(const string & indent, unsigned int depth = 1);

		void write(ostream & os) const;

	private:
		const string & indent;
		unsigned int depth;
	};
}
#endif
