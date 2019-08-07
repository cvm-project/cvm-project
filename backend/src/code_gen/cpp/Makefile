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
CXXFLAGS = -fPIC -flto -std=c++17 -fopenmp
LDFLAGS = -fuse-ld=gold -flto -L$(BACKEND_DIR)/build -Wl,-rpath=$(BACKEND_DIR)/build -lruntime -fopenmp

ifdef LIBOMPDIR
    LDFLAGS += -L$(LIBOMPDIR)
endif

DEBUG ?= 0
ifeq ($(DEBUG), 1)
    CXXFLAGS += -g3 -DDEBUG
    LDFLAGS += -g
    LIB = libexecute.dbg.so
else
    CXXFLAGS += -DNDEBUG -O3 -ffast-math -march=native -mtune=native
    LIB = libexecute.opt.so
endif

LLVM_ASAN ?= 0
ifeq ($(LLVM_ASAN), 1)
    CXXFLAGS += -fsanitize=address,undefined -fno-sanitize=vptr,alignment
    LDFLAGS += -fsanitize=address,undefined -fno-sanitize=vptr,alignment
endif

# Set inputs
UDF_SOURCES = llvm_funcs.ll
UDF_OBJECTS = $(UDF_SOURCES:.ll=.o)

MAINHEADER = $(SCRIPT_DIR)/operators/Operators.h
HEADERS = $(filter-out $(MAINHEADER),$(wildcard $(SCRIPT_DIR)/operators/*.h))
GCHEADER = $(SCRIPT_DIR)/operators/Operators.h.gch
GENHEADER = $(SCRIPT_DIR)/operators/Operators.h.sh

# Rules
all: $(LIB)

.PRECIOUS: %.ll

%.o: %.ll
	$(CXX) -c $< -o $@ $(CXXFLAGS) -Wno-override-module

execute.opt.cpp: execute.cpp
	cp $^ $@

execute.dbg.cpp: execute.cpp
	clang-format-7.0 $< > $@

execute.opt.o: execute.opt.cpp $(GCHEADER)
	$(CXX) -c $< -o $@ $(CXXFLAGS) $(INC) -include-pch $(GCHEADER)

execute.dbg.o: execute.dbg.cpp $(MAINHEADER)
	$(CXX) -c $< -o $@ $(CXXFLAGS) $(INC)

$(GCHEADER): $(MAINHEADER)
	$(CXX) $(CXXFLAGS) $(INC) -x c++-header $< -o $@

$(MAINHEADER): $(HEADERS)
	$(GENHEADER) $@ $^

libexecute.%.so: $(UDF_OBJECTS) execute.%.o
	 $(CXX) $^ -o $@ -shared $(LDFLAGS)

execute.s: $(LIB)
	objdump -D $^ > $@

clean:
	rm -f *.bc *.asm *.o *.so $(GCHEADER) $(MAINHEADER)

.PHONY: clean
