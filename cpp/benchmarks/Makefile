CC = clang
FLAGS = -O3 -std=c++14 -lstdc++ -ffast-math
all: main

main: main.cpp
	$(CC)  $^ $(FLAGS) -o main

clean:
	rm -f *.bc *.ll *.o main