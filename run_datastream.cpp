#include "json_formatter.h"
#include "model.h"
#include "datastream.h"

namespace datastream {
	const int arg_schema_sets_file_name = 1;
	const int arg_schema_elements_file_name = 2;

	const string usage(
		"usage:\n"
		"datapipe schema_file element_file\n"
		"options:"
		"-x format xml (default is json)\n"
		"-p pretty print (default is compact)\n"
	);
}

int main(int argc, char *argv[]){

	using namespace datastream;

	const regex argument_flag_pattern = regex (
		rx_start + "(-)" + rx_any
	);

	string schema_file_path = "";
	string element_file_path = "";

	std::regex flag_pattern ("[xp]");
	std::regex flag_anti_pattern ("[^-xp]");
	bool format_set = false;
	bool style_set = false;
	bool schema_file_set = false;
	bool element_file_set = false;

	Format format = Format::json;
	Style  style = Style::compact;

	try {
		// if (argc < 3){
		// 	throw std::invalid_argument("schema and schema elements filenames required");
		// }

		for (int i = 1; i < argc; i++)
		{
			string argument_string = argv[i];

			std::smatch argument_match;
			std::regex_match(
				argument_string,
				argument_match,
				argument_flag_pattern
			);

			if (argument_match.length()){


				std::smatch flag_match;

				if (std::regex_search (argument_string, flag_match, flag_anti_pattern)){
					throw std::invalid_argument(usage);
				}

				while (std::regex_search (argument_string, flag_match, flag_pattern)){

					if(flag_match[0]=="x"){
						if(format_set){
							throw std::invalid_argument(usage);
						}
						format = Format::xml;
						format_set = true;

					} else if(flag_match[0]=="p") {
						if(style_set){
							throw std::invalid_argument(usage);
						}
						style = Style::pretty;
						style_set = true;
					}
					argument_string = flag_match.suffix().str();
				}
			}
			else
			{
				if(!schema_file_set){

					schema_file_path = argument_string;
					//std::cout << "schema file :" << argument_string << std::endl;
					schema_file_set = true;

				} else if (!element_file_set){

					element_file_path = argument_string;
					//std::cout << "element file :" << argument_string << std::endl;
					element_file_set = true;

				} else {
					throw std::invalid_argument(usage);
				}
			}
		}

		if (!element_file_set || !schema_file_set){
			throw std::invalid_argument("schema and schema elements filenames required\n" + usage);
		}

		jsonFormatter formatter;

		Model model(
			schema_file_path,
			element_file_path
		);

		std::cout << Datastream(model,formatter);
	}
	catch( const std::exception& e ){
		 std::cerr << e.what();
		 return -1;
	 }

	return 0;
}
