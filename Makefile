# Compiler options
OPTIONS = -O3

# Default target
all: build/main

# Debug mode target
debug: OPTIONS = -O0 -g
debug: build/main

# Compile executable files
build/main: build/ai.o build/common_functions.o build/interactive.o tools/main.c
	gcc $(OPTIONS) tools/main.c -o build/main -Iincludes build/ai.o build/common_functions.o build/interactive.o

# Compile object files
build/ai.o: src/ai.c includes/ai.h
	gcc $(OPTIONS) -c src/ai.c -Iincludes -o build/ai.o

build/common_functions.o: src/common_functions.c includes/common_functions.h
	gcc $(OPTIONS) -c src/common_functions.c -Iincludes -o build/common_functions.o

build/interactive.o: src/interactive.c includes/interactive.h
	gcc $(OPTIONS) -c src/interactive.c -Iincludes -o build/interactive.o

# Clean compiled files
clean:
	rm -rf build/*.o build/main
