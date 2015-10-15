CC=g++
CFLAGS= -std=c++11
LDFLAGS=
SOURCES=run_datastream.cpp data.cpp data_set.cpp data_row.cpp data_element.cpp schema.cpp schema_set.cpp schema_element.cpp
/OBJECTS=$(SOURCES:.cpp.o)

RUN_PARAMETERS =  ./sample_data/schema.csv ./sample_data/schema_element.csv | tee output.json
# OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=../run_datastream

# all: $(SOURCES) $(EXECUTABLE)

# $(EXECUTABLE): $(OBJECTS)
#     $(CC) $(LDFLAGS) $(OBJECTS) -o $@

# .cpp.o:
#     $(CC) $(CFLAGS) $< -o $@


.PHONY: clean all build run clearscreen

all:	clearscreen clean build run

clearscreen:
		clear

build:
		$(CC) $(CFLAGS) $(SOURCES) -o $(EXECUTABLE)

clean:
		rm -f $(EXECUTABLE)
run:
		$(EXECUTABLE) $(RUN_PARAMETERS)
