CC = clang-3.7
AS = llvm-as-3.7
FLAGS = -O3 -std=c++14 -ffast-math -flto

SOURCES = execute.cpp $(wildcard ../src/operators/*.cpp)
INC = -I ../src/ -I functions_llvm -I ../src/utils/
UDF_SOURCES = $(wildcard functions_llvm/*.ll)

OBJECTS = $(SOURCES:.cpp=.o)
UDF_OBJECTS = $(UDF_SOURCES:.ll=.o)


all: execute


.PRECIOUS: %.ll


c_execute.o: c_execute.c
	$(CC) -c $^ -o $@ -fPIC

%.o: %.cpp
	$(CC) -c $^ $(INC) -o $@ $(FLAGS)

%.o: %.ll
	$(CC) -c $^ -o $@ -O3 -flto -ffast-math


execute: $(UDF_OBJECTS) $(OBJECTS) c_execute.o
	$(CC) $^ -o $@.so -shared $(FLAGS) -lstdc++

#%.asm: %
#	objdump -D $^ > $@

clean:
	rm -f *.bc *.ll *.o *.so execute *.asm functions_llvm/*.bc

.PHONY: clean