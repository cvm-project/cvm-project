CC = clang
FLAGS = -O3 -std=c++14 -lstdc++ -ffast-math -I ../src -march=native
all: main

main: main.cpp
	$(CC)  $^ $(FLAGS) -o main


main.s: main.cpp
	objdump -D main.o


main.ll: main.cpp
	$(CC) $^ $(FLAGS) -S -emit-llvm -o $@

clean:
	rm -f *.bc *.ll *.o main