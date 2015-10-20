#include "quote.h"

namespace datastream {

	Quote::Quote(const string & text, const string & quote):
	text(text), quote(quote){}

	void Quote::write(ostream & os) const {
		os << quote << text << quote;
	}
}
