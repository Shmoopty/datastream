#include "json_formatter.h"
#include "json_compact_formatter.h"
#include "model.h"
#include "datastream.h"


namespace datastream {
	const string usage(
		"usage:\n"
		"datapipe schema_file element_file\n"
		"options:"
		"    format:\n"
		"        -j json (default)\n"
		"        -x xml\n"
		"    style:\n"
		"        -c compact (default)\n"
		"        -p pretty print\n"
		// a potential enhancement for another day !
		//"    structure:\n"
		//"        -t tree (default)\n"
		//"        -d denormalised\n"
		//"        -f flat record sets\n"
	);
}

using namespace datastream;

template <typename T>
std::string concatenateFlags(const std::map<char,T> & m){
	std::string s;
	for (auto & f : m){
		s += f.first;
	}
	return s;
}

template <typename T>
void setter(const char ch, bool & isSet, T & val, const std::map<char,T> & m){
	auto search = m.find(ch);

	if (isSet || search == m.end()){
		throw std::invalid_argument(datastream::usage);
	}
	val = search->second;
}

void readArgs(
	int argc,
	char *argv[],
	string & schema_file_path,
	string & element_file_path,
	Format & format,
	Style & style
){
	const regex dash_flag_pattern = regex (
		rx_start + "(-)" + rx_any
	);

	string format_flags = concatenateFlags(format_map);
	string style_flags = concatenateFlags(style_map);
	//string structure_flags = concatenateFlags(structure_map);

	bool format_set = false;
	bool style_set = false;
	//bool structure_set = false;

	bool schema_file_set = false;
	bool element_file_set = false;
	//Structure  structure = Structure::tree;

	for (int i = 1; i < argc; i++)
	{
		string argument_string = argv[i];

		std::smatch dash_flag_match;
		std::regex_match(argument_string, dash_flag_match,dash_flag_pattern);

		if (!dash_flag_match.length()){
			if(!schema_file_set){
				schema_file_path = argument_string;
				schema_file_set = true;
			} else if (!element_file_set){
				element_file_path = argument_string;
				element_file_set = true;
			} else {
				throw std::invalid_argument(usage);
			}
		}
		else
		{
			if (argument_string.length()==1){
				throw std::invalid_argument(usage);
			}

			std::string::iterator it = argument_string.begin();
			it++; //skip dash

			while (it < argument_string.end()){
				if (format_flags.find(*it)!=std::string::npos){
					setter(*it, format_set, format, format_map);
				}
				else if (style_flags.find(*it)!=std::string::npos){
					setter(*it, style_set, style, style_map);
				}
				// else if (structure_flags.find(*it)!=std::string::npos){
				// 	setter(*it, structure_set, structure, structure_map);
				// }
				else{
					throw std::invalid_argument(usage);
				}
				it++;
			}
		}
	}

	if (!element_file_set || !schema_file_set){
		throw std::invalid_argument("schema and schema elements filenames required\n" + usage);
	}
}

Formatter * createFormatter(
	Format format,
	Style style
){

	if (style == Style::compact){
		return new jsonCompactFormatter();
	}
	else {
		return new jsonFormatter();
	}
}

int main(int argc, char *argv[]){

	using namespace datastream;

	try {

		string schema_file_path = "";
		string element_file_path = "";

		Format format = Format::json;
		Style style = Style::compact;

		readArgs(argc, argv, schema_file_path, element_file_path, format, style );

		Formatter * formatter = createFormatter(format, style);

		Model model(
			schema_file_path,
			element_file_path
		);

		std::cout << Datastream(model,*formatter);

		delete formatter;

	}
	catch( const std::exception& e ){
		 std::cerr << e.what();
		 return -1;
	 }

	return 0;
}
