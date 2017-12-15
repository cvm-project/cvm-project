# Load local config, if it exists
SCRIPT_DIR:=$(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
-include $(SCRIPT_DIR)/Makefile.local

# Set to clang if not overwritten
ifeq ($(origin CC),default)
    CC = clang
endif
ifeq ($(origin CXX),default)
    CXX = clang++
endif

# Set flags
INC = -I ../src/ -I ../src/utils/
CFLAGS = -DNDEBUG -O3 -ffast-math -march=native -mtune=native -fPIC -flto
CXXFLAGS = $(CFLAGS) -std=c++14
LDFLAGS = -fuse-ld=gold -flto

# Set inputs
UDF_SOURCES = llvm_funcs.ll
UDF_OBJECTS = $(UDF_SOURCES:.ll=.o)

HEADER = ../src/operators/Operators.h
GCHEADER = ../src/operators/Operators.h.gch
GENHEADER = ../src/operators/Operators.h.sh

# Rules
all: execute.so

.PRECIOUS: %.ll

%.o: %.ll
	$(CXX) -c $< -o $@ $(CXXFLAGS) -Wno-override-module

execute.o: execute.cpp $(GCHEADER)
	$(CXX) -c $< -o $@ $(CXXFLAGS) $(INC) -include-pch $(GCHEADER)

$(GCHEADER): $(HEADER)
	$(CXX) $(CXXFLAGS) $(INC) -x c++-header $< -o $@

$(HEADER):
	$(GENHEADER) $@

execute.so: $(UDF_OBJECTS) execute.o
	 $(CXX)  $^ -o execute${LIB_ID}.so -shared $(LDFLAGS)

execute.s: execute.so
	objdump -D $^ > $@

clean:
	rm -f *.bc execute *.asm functions_llvm/*.o *.o *.so *.ll $(GCHEADER) $(HEADER)

.PHONY: clean
