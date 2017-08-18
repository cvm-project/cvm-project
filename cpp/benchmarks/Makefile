CC = clang-3.7
#FLAGS = -O3 -std=c++14 -lstdc++ -march=native -v -ffast-math
FLAGS = -O3 -std=c++14 -lstdc++ -ffast-math
all: main

main: main.cpp
	$(CC)  $^ $(FLAGS) -o main

clean:
	rm -f *.bc *.ll *.o main