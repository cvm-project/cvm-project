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

BACKEND_DIR = $(SCRIPT_DIR)/../../../

# Set flags
INC = -I$(SCRIPT_DIR)/operators -I$(BACKEND_DIR)/src
CFLAGS = -DNDEBUG -O3 -ffast-math -march=native -mtune=native -fPIC -flto
CXXFLAGS = $(CFLAGS) -std=c++14
LDFLAGS = -fuse-ld=gold -flto -L$(BACKEND_DIR)/build -lruntime

# Set inputs
UDF_SOURCES = llvm_funcs.ll
UDF_OBJECTS = $(UDF_SOURCES:.ll=.o)

MAINHEADER = $(SCRIPT_DIR)/operators/Operators.h
HEADERS = $(filter-out $(MAINHEADER),$(wildcard $(SCRIPT_DIR)/operators/*.h))
GCHEADER = $(SCRIPT_DIR)/operators/Operators.h.gch
GENHEADER = $(SCRIPT_DIR)/operators/Operators.h.sh

# Rules
all: execute.so

.PRECIOUS: %.ll

%.o: %.ll
	$(CXX) -c $< -o $@ $(CXXFLAGS) -Wno-override-module

execute.o: execute.cpp $(GCHEADER)
	$(CXX) -c $< -o $@ $(CXXFLAGS) $(INC) -include-pch $(GCHEADER)

$(GCHEADER): $(MAINHEADER)
	$(CXX) $(CXXFLAGS) $(INC) -x c++-header $< -o $@

$(MAINHEADER): $(HEADERS)
	$(GENHEADER) $@

execute.so: $(UDF_OBJECTS) execute.o
	 $(CXX)  $^ -o execute${LIB_ID}.so -shared $(LDFLAGS)

execute.s: execute.so
	objdump -D $^ > $@

clean:
	rm -f *.bc *.asm *.o *.so $(GCHEADER) $(MAINHEADER)

.PHONY: clean
