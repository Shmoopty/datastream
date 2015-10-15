#ifndef datastream_h
#define datastream_h

#include <regex>
#include <string>
#include <iostream>
#include <fstream>

#include <map>
#include <vector>
#include <list>
#include <stdexcept>
#include <sstream>
#include "formatter.h"
#include "model.h"

namespace datastream {

	class Datastream{

	public:
		Datastream() = default;

		Datastream(Model& model, Formatter& formatter):
		model(model),
		formatter(formatter)
		{};

		friend std::ostream& operator<<(std::ostream& os, const Datastream& obj)
		{
			obj.model.write(os, obj.formatter);
			return os;
		}

	private:
		Model& model;
		Formatter& formatter;

	};
}
#endif
