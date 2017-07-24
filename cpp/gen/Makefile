CC = clang-3.7
AS = llvm-as-3.7

SOURCES = execute.cpp $(wildcard ../src/operators/*.cpp)
INC = -I ../src/ -I functions_llvm
UDF_SOURCES = $(wildcard functions_llvm/*.ll)

OBJECTS = $(SOURCES:.cpp=.bc)
UDF_OBJECTS = $(UDF_SOURCES:.ll=.bc)


all: execute


.PRECIOUS: %.ll

%.ll: %.cpp
	$(CC) -S -O3 -emit-llvm $^ -std=c++14  $(INC) -o $@

%.bc: %.ll
	$(AS) $^ -o $@


execute: $(UDF_OBJECTS) $(OBJECTS)
	$(CC) -flto $^ -o $@.so -lstdc++ -shared

#%.asm: %
#	objdump -D $^ > $@

clean:
	rm -f *.bc *.ll execute *.asm functions_llvm/*.bc

.PHONY: clean