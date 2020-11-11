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
ifeq ($(CXXFORMAT),)
    CXXFORMAT := cat
endif
ifeq ($(LLVMLINK),)
    LLVMLINK := llvm-link
endif
ifeq ($(LLVMOPT),)
    LLVMOPT := opt
endif

BACKEND_DIR = $(realpath $(SCRIPT_DIR)/../../../../)

# Set flags
INC = -I$(SCRIPT_DIR)/operators -I$(BACKEND_DIR)/src/runtime/include
CXXFLAGS = -fPIC -std=c++17 -fopenmp -D_GLIBCXX_USE_CXX11_ABI
LDFLAGS = -fPIC -L$(BACKEND_DIR)/build -Wl,-rpath=$(BACKEND_DIR)/build -lruntime -fopenmp
OPTFLAGS =

ifdef LIBOMPDIR
    LDFLAGS += -L$(LIBOMPDIR)
endif

DEBUG ?= 0
ifeq ($(DEBUG), 1)
    CXXFLAGS += -g3 -DDEBUG
    LDFLAGS += -g
    OPTFLAGS +=
    LIB = libexecute.dbg.so
else
    CXXFLAGS += -DNDEBUG -O3 -ffast-math -march=native -mtune=native
    OPTFLAGS += -O3 -ffast-math -march=native
    LIB = libexecute.opt.so
endif

WITH_LLVM_ASAN ?= 0
ifeq ($(WITH_LLVM_ASAN), 1)
    CXXFLAGS += -fsanitize=address,undefined -fno-sanitize=vptr,alignment
endif

# Set inputs
UDF_SOURCES = $(wildcard llvm_funcs/*.ll)

MAINHEADER = $(SCRIPT_DIR)/operators/AllHeaders.h
OPERATOR_HEADERS = $(filter-out $(MAINHEADER),$(wildcard $(SCRIPT_DIR)/operators/*.h))
RUNTIME_HEADERS = $(shell find $(BACKEND_DIR)/src/runtime/include/runtime/jit -name "*.hpp")
HEADERS = $(OPERATOR_HEADERS) $(RUNTIME_HEADERS)
GCHEADER = $(SCRIPT_DIR)/operators/AllHeaders.h.gch
GENHEADER = $(SCRIPT_DIR)/operators/AllHeaders.h.sh

# Rules
all: $(LIB)

.PRECIOUS: %.ll

%.o: %.ll
	$(CXX) -c $< -o $@ $(CXXFLAGS) -Wno-override-module

execute.opt.cpp: execute.cpp
	cp $^ $@

execute.dbg.cpp: execute.cpp
	$(CXXFORMAT) $< > $@

execute.opt.bc: execute.opt.cpp $(GCHEADER)
	$(CXX) -c $< -o $@ $(CXXFLAGS) -emit-llvm $(INC) -include-pch $(GCHEADER)

execute.dbg.bc: execute.dbg.cpp $(MAINHEADER)
	$(CXX) -c $< -o $@ $(CXXFLAGS) -emit-llvm $(INC)

$(GCHEADER): $(MAINHEADER)
	$(CXX) $(CXXFLAGS) $(INC) -x c++-header $< -o $@

$(MAINHEADER): $(HEADERS)
	$(GENHEADER) $@ $^

libexecute.opt.linked.bc: $(UDF_SOURCES) execute.opt.bc
libexecute.dbg.linked.bc: $(UDF_SOURCES) execute.dbg.bc
libexecute.%.linked.bc:
	$(LLVMLINK) $^ -o $@

libexecute.%.bc: libexecute.%.linked.bc
	$(LLVMOPT) $(OPTFLAGS) $^ -o $@

libexecute.%.so: libexecute.%.bc
	 $(CXX) $^ -o $@ -shared $(LDFLAGS)

execute.s: $(LIB)
	objdump -D $^ > $@

clean:
	rm -f *.bc *.asm *.o *.so $(GCHEADER) $(MAINHEADER)

.PHONY: clean
