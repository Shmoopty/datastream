CC=g++-5
CFLAGS= -c -std=c++11 -Wall
LDFLAGS= -pthread

SOURCES=main.cpp datastream.cpp data.cpp model.cpp data_set.cpp data_row.cpp data_element.cpp schema.cpp schema_set.cpp schema_element.cpp xml_compact_formatter.cpp xml_formatter.cpp json_compact_formatter.cpp json_formatter.cpp formatter.cpp indent.cpp quote.cpp

RUN_PARAMETERS_JSON =  ./sample_data/schema.csv ./sample_data/schema_element.csv -jp | tee ./output/output.json
RUN_PARAMETERS_JSON_COMPACT =  ./sample_data/schema.csv ./sample_data/schema_element.csv -jc > ./output/output_compact.json
RUN_PARAMETERS_XML =  ./sample_data/schema.csv ./sample_data/schema_element.csv -xp | tee ./output/output.xml
RUN_PARAMETERS_XML_COMPACT =  ./sample_data/schema.csv ./sample_data/schema_element.csv -xc > ./output/output_compact.xml

OBJECTS=$(SOURCES:.cpp=.o)

EXECUTABLE=./ds

.PHONY: all clearscreen clean_before build clean_after run

all:	clearscreen clean_before build clean_after run

clearscreen:
		clear

build: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
		$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
		$(CC) $(CFLAGS) $< -o $@

clean_before:
		rm -f $(EXECUTABLE)

clean_after:
		rm *.o
run:
		$(EXECUTABLE) $(RUN_PARAMETERS_JSON_COMPACT)
		$(EXECUTABLE) $(RUN_PARAMETERS_XML_COMPACT)
		$(EXECUTABLE) $(RUN_PARAMETERS_JSON)
		$(EXECUTABLE) $(RUN_PARAMETERS_XML)
