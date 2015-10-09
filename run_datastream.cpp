#include "json_formatter.h"
#include "model.h"
#include "datastream.h"

namespace datastream {
	const int arg_schema_sets_file_name = 1;
	const int arg_schema_elements_file_name = 2;
}

int main(int argc, char *argv[]){

	using namespace datastream;

	try {
		if (argc < 3){
			throw std::invalid_argument("schema and schema elements filenames required");
		}

		jsonFormatter formatter;

		Model model(
			argv[arg_schema_sets_file_name],
			argv[arg_schema_elements_file_name]
		);

		std::cout << Datastream(model,formatter);
	}
	catch( const std::exception& e ){
		 std::cout << e.what();
		 return -1;
	 }

	return 0;
}
