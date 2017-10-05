CC = clang
FLAGS = -O3  -flto -ffast-math -march=native

SOURCES = execute.cpp
INC = -I ../src/ -I ../src/utils/
UDF_SOURCES = $(wildcard functions_llvm/*.ll)
PCH = -include-pch ../src/operators/Operators.h.gch

OBJECTS = $(SOURCES:.cpp=.o)
UDF_OBJECTS = $(UDF_SOURCES:.ll=.o)

all: execute.so

.PRECIOUS: %.ll

%.o: %.cpp
	$(CC) -c $^ $(INC) -o $@ $(FLAGS) -std=c++14 $(PCH)

%.o: %.ll
	$(CC) -c $^ -o $@ $(FLAGS) -Wno-override-module

c_execute.o: c_execute.c
	 $(CC) -c $^ -o $@ -fPIC $(FLAGS)


execute.so: $(UDF_OBJECTS) $(OBJECTS) c_execute.o
	 $(CC)  $^ -o execute${LIB_ID}.so -shared -lstdc++ $(FLAGS)

execute.s: execute.so
	objdump -D $^ > $@

execute.ll: execute.cpp
	$(CC) $^ -o $@ -S -emit-llvm $(FLAGS) $(INC) -std=c++14


clean:
	rm -f *.bc execute *.asm functions_llvm/*.bc *.o *.so

.PHONY: clean
