CC = clang-3.7
AS = llvm-as-3.7

SOURCES = main.cpp execute.cpp $(wildcard ../src/operators/*.cpp)
INC = -I ../src/ -I functions_llvm
UDF_SOURCES = $(wildcard functions_llvm/*.ll)

OBJECTS = $(SOURCES:.cpp=.bc)
UDF_OBJECTS = $(UDF_SOURCES:.cpp=.bc)

all: main main.asm


.PRECIOUS: %.ll

%.ll: %.cpp
	$(CC) -S -O3 -emit-llvm $^ -std=c++14  $(INC) -o $@

%.bc: %.ll
	$(AS) $^ -o $@

main: $(UDF_OBJECTS) $(OBJECTS)
	$(CC) -flto $^ -o $@ -lstdc++

%.asm: %
	objdump -D $^ > $@

clean:
	rm -f *.bc *.ll main *.asm

.PHONY: clean