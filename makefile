.PHONY: clean all build run clearscreen

all:	clearscreen clean build run

clearscreen:
		clear
build:
		g++ -std=c++11 run_datastream.cpp -o ../run_datastream
clean:
		rm -f ../run_datastream
run:
		../run_datastream ./sample_data/schema.csv ./sample_data/schema_element.csv | tee output_02.json
