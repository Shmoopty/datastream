#ifndef datastream_formatter_quote
#define datastream_formatter_quote

#include "datastream_definitions.h"

#include <iostream>

namespace datastream {

	using std::ostream;

	class Formatter;

	class Quote{
	public:
		friend ostream& operator<<(std::ostream& os, const Quote& obj){
			obj.write(os);
		  return os;
		}
		Quote() = delete;

		Quote(const string & text, const string & quote);

		void write(ostream & os) const;

	private:
		const string & text;
		const string & quote;
	};
}
#endif
