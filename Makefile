CC = gcc
COMP_FLAGS =
LOAD_FLAGS = $(COMP_FLAGS) -lfl -ldl -lm


HEADERS = $(wildcard src/cryml/*.h) $(wildcard src/cryml/stdlib/*.h) $(wildcard src/cryml/data/*.h)
CRYML_OBJECTS  := $(patsubst %.c,%.o,$(wildcard src/cryml/*.c))
DATA_OBJECTS   := $(patsubst %.c,%.o,$(wildcard src/cryml/stdlib/*.c))
STDLIB_OBJECTS := $(patsubst %.c,%.o,$(wildcard src/cryml/data/*.c))

CRYML_FILES  := src/cryml/*.c
DATA_FILES   := src/cryml/stdlib/*.c
STDLIB_FILES := src/cryml/data/*.c

SYNTACTIC_OBJECTS := src/generated/syntactic.o


default: parser

src/generated/lexical.c: $(HEADERS) src/grammar/cryml.lex folder_dependency
	flex --outfile=src/generated/lexical.c src/grammar/cryml.lex

syntactic: src/generated/syntactic.c

src/generated/syntactic.c: src/grammar/cryml.bison folder_dependency
	bison --report=state --graph --defines=src/generated/syntactic.h -o src/generated/syntactic.c src/grammar/cryml.bison

graph: syntactic
	dot -Tpng src/generated/syntactic.dot -o src/generated/graph.png
	dot -Tsvg src/generated/syntactic.dot -o src/generated/graph.svg

parser: bin/parser

bin/parser: bin/lib_example_functions.so $(HEADERS) src/generated/lexical.c $(CRYML_OBJECTS) $(DATA_OBJECTS) $(STDLIB_OBJECTS) $(SYNTACTIC_OBJECTS) src/parser.c
	$(CC) -g $(CRYML_OBJECTS) $(DATA_OBJECTS) $(STDLIB_OBJECTS) $(SYNTACTIC_OBJECTS) src/parser.c -o bin/parser $(LOAD_FLAGS)

cryml: bin/cryml

bin/cryml: $(HEADERS) src/generated/lexical.c $(CRYML_OBJECTS) $(DATA_OBJECTS) $(STDLIB_OBJECTS) $(SYNTACTIC_OBJECTS) src/parser.c
	$(CC) -g $(CRYML_OBJECTS) $(DATA_OBJECTS) $(STDLIB_OBJECTS) $(SYNTACTIC_OBJECTS) src/cryml.c -o bin/cryml $(LOAD_FLAGS)

debug_parser: $(HEADERS) src/generated/lexical.c $(CRYML_OBJECTS) $(DATA_OBJECTS) src/parser.c syntactic
	$(CC) -g $(CRYML_OBJECTS) $(DATA_OBJECTS) $(STDLIB_OBJECTS) src/generated/syntactic.c src/parser.c -o bin/parser $(LOAD_FLAGS)

compile: $(HEADERS) src/generated/lexical.c $(CRYML_OBJECTS) $(DATA_OBJECTS) $(STDLIB_OBJECTS) src/cryml.c
	$(CC) $(CRYML_OBJECTS) $(DATA_OBJECTS) $(STDLIB_OBJECTS) src/cryml.c -o bin/cryml $(LOAD_FLAGS)

bin/unit_test: $(HEADERS) src/generated/lexical.c $(CRYML_OBJECTS) $(DATA_OBJECTS) $(STDLIB_OBJECTS) test/unit/*.c test/unit.c syntactic
	$(CC) -g $(CRYML_OBJECTS) $(DATA_OBJECTS) $(STDLIB_OBJECTS) test/unit/*.c test/unit.c src/generated/syntactic.c -lcunit -o bin/unit_test $(LOAD_FLAGS)

unit_test: $(HEADERS) src/generated/lexical.c $(CRYML_OBJECTS) $(DATA_OBJECTS) $(STDLIB_OBJECTS) test/unit/*.c test/unit.c bin/unit_test
	./bin/unit_test

test: folder_dependency $(HEADERS) src/generated/lexical.c $(CRYML_OBJECTS) $(DATA_OBJECTS) test/unit/*.c test/unit.c bin/unit_test
	./bin/unit_test

parsing_leaks: parser test/examples/simple.cryml
	MallocStackLogging=1 ./bin/parser test/examples/simple.cryml

testing_leaks: unit_test
	MallocStackLogging=1 ./bin/unit_test

bin/lib_example_functions.so: $(CRYML_FILES) $(DATA_FILES) $(STDLIB_FILES)
	$(CC) -Wall -fPIC -c $(CRYML_FILES) $(DATA_FILES) $(STDLIB_FILES) test/lib_examples/lib_example_functions.c
	$(CC) -shared -o bin/lib_example_functions.so *.o
	rm *.o

bin/ray_tracing.so: $(CRYML_FILES) $(DATA_FILES) $(STDLIB_FILES)
	$(CC) -Wall -fPIC -c $(CRYML_FILES) $(DATA_FILES) $(STDLIB_FILES) test/lib_examples/ray_tracing.c
	$(CC) -shared -o bin/ray_tracing.so *.o
	rm *.o

clean:
	rm -rfv src/generated/* bin/* $(CRYML_OBJECTS) $(DATA_OBJECTS) $(STDLIB_OBJECTS) $(SYNTACTIC_OBJECTS)

folder_dependency:
	mkdir -p src/generated
	mkdir -p tmp
	mkdir -p bin

