CC = clang-3.7
AS = llvm-as-3.7

SOURCES = main.cpp BuildDAG.cpp ../src/operators/*.cpp
INC = -I ../src/ -I functions_llvm

all: main main.asm


.PRECIOUS: %.ll

%.ll: $(SOURCES)
	$(CC) -S -O3 -emit-llvm $^ -std=c++14  $(INC)

%.bc: %.ll
	$(AS) $^ -o $@

main: main.bc BuildDAG.bc MapOperator.bc CollectionSourceOperator.bc functions_llvm/llvm_map_function0.bc
	$(CC) -flto $^ -o $@ -lstdc++ -v

%.asm: %
	objdump -D $^ > $@

clean:
	rm -f *.bc *.ll main *.asm

.PHONY: clean