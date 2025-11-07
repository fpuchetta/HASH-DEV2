VALGRIND_FLAGS=--leak-check=full --track-origins=yes --show-reachable=yes --error-exitcode=2 --show-leak-kinds=all --trace-children=yes
CFLAGS =-std=c99 -Wall -Wconversion -Wtype-limits -pedantic -Werror -O2 -g -Isrc
SRC = src/*.c
CC = gcc

all: clean valgrind-alumno

hash: $(SRC) main.c
	$(CC) $(CFLAGS) $^ -o $@

valgrind-alumno: pruebas_alumno
	valgrind $(VALGRIND_FLAGS) ./pruebas_alumno

pruebas_alumno: src/*.c pruebas_alumno.c
	$(CC) $(CFLAGS) src/*.c pruebas_alumno.c -o pruebas_alumno

clean:
	rm -f pruebas_alumno hash

format:
	clang-format -i -style=file $(SRC) src/*.h *.c *.h