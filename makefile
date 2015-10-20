CC=g++
CFLAGS= -std=c++11 -Wall
LDFLAGS=
#
SOURCES=run_datastream.cpp datastream.cpp data.cpp model.cpp data_set.cpp data_row.cpp data_element.cpp schema.cpp schema_set.cpp schema_element.cpp xml_compact_formatter.cpp xml_formatter.cpp json_compact_formatter.cpp json_formatter.cpp formatter.cpp indent.cpp quote.cpp

RUN_PARAMETERS =  ./sample_data/schema.csv ./sample_data/schema_element.csv -jp | tee output.json
OBJECTS=$(SOURCES:.cpp=.o)
#../run_datastream ./sample_data/schema.csv ./sample_data/schema_element.csv -p | tee output.json
EXECUTABLE=../ds

.PHONY: clean all build run clearscreen

all:	clearscreen clean build run

clearscreen:
		clear

build:
	$(CC) $(CFLAGS) $(SOURCES) -o $(EXECUTABLE)
#	$(SOURCES) $(EXECUTABLE)
#

# $(EXECUTABLE): $(OBJECTS)
# 		$(CC) $(LDFLAGS) $(OBJECTS) -o -v $@
#
# .cpp.o:
# 		$(CC) $(CFLAGS) $< -o $@

clean:
		rm -f $(EXECUTABLE)
run:
		$(EXECUTABLE) $(RUN_PARAMETERS)
