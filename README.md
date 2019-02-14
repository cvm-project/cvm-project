## JITQ - Just-In-Time Query Compilation
JITQ is a dataflow framework featuring just-in-time compilation and query optimization in presence of user-defined functions.

## Installation

0. Git submodules
```
git submodule update --recursive --init
```

1. Python requirements<br/>
```
pip install -r requirements.txt
```


2. Dev requirements<br/>
```
sudo apt-get install build-essential \
        cmake \
        gcc-5 \
        g++-5 \
        libboost-all-dev \
        libboost-dev \
        wget \
        xz-utils
```

3. Gold linker<br/>
```
sudo apt install binutils-dev ninja-build python-minimal
```
Download and build with ninja
```
cd /tmp/ && \
wget http://releases.llvm.org/5.0.0/llvm-5.0.0.src.tar.xz && \
tar -xf llvm-5.0.0.src.tar.xz && \
mkdir /tmp/llvm-5.0.0.src/build && \
cd /tmp/llvm-5.0.0.src/build && \
CXX=g++-5 CC=gcc-5 cmake -G Ninja ../ \
    -DLLVM_BINUTILS_INCDIR=/usr/include \
    -DLLVM_TARGETS_TO_BUILD=X86 \
    -DCMAKE_BUILD_TYPE=MinSizeRel \
    -DLLVM_OPTIMIZED_TABLEGEN=ON && \
ninja LLVMgold
```

4. Clang <br>
download, build and add links to all tools<br/>
```
mkdir /opt/clang+llvm-5.0.0/ && \
cd /opt/clang+llvm-5.0.0/ && \
wget http://releases.llvm.org/5.0.0/clang+llvm-5.0.0-linux-x86_64-ubuntu16.04.tar.xz -O - \
     | tar -x -I xz --strip-components=1 && \
for file in bin/*; \
do \
    ln -s $PWD/$file /usr/bin/$(basename $file)-5.0; \
done
```
Copy goldlinker<br/>
```
cp /tmp/llvm-5.0.0.src/build/lib/LLVMgold.so /opt/clang+llvm-5.0.0/lib
```

5. CMake<br/>
```
mkdir /opt/cmake-3.10.6/ && \
cd /opt/cmake-3.10.6/ && \
wget https://cmake.org/files/v3.10/cmake-3.10.0-Linux-x86_64.tar.gz -O - \
    | tar -xz --strip-components=1 && \
for file in bin/*; \
do \
    ln -s $PWD/$file /usr/bin/$(basename $file)-3.10; \
done
```

6. cppcheck<br/>
```
sudo apt install build-essential \
        libpcre3-dev \
        unzip
```
Download and build<br/>
```
cd /tmp/ && \
wget https://github.com/danmar/cppcheck/archive/1.81.zip && \
unzip 1.81.zip && \
cd /tmp/cppcheck-1.81 && \
make SRCDIR=build CFGDIR=/opt/cppcheck-1.81/share/cfg HAVE_RULES=yes \
     CXXFLAGS="-O2 -DNDEBUG -Wall -Wno-sign-compare -Wno-unused-function" && \
mkdir -p /opt/cppcheck-1.81/bin /opt/cppcheck-1.81/share && \
cp -r cfg /opt/cppcheck-1.81/share && \
cp cppcheck /opt/cppcheck-1.81/bin/cppcheck-1.81;
```
Add links
```
for bin in /opt/cppcheck-1.81/bin/cppcheck-1.81; do \
        ln -s $bin /usr/bin/; \
    done
```

7. GraphViz<br/>
```
sudo apt-get install libgraphviz-dev
```

## Development
1. Build<br>
```
cd jitq/backend/build
cmake ../src/
# or whatever binary you installed above, such as:
cmake-v3.10 ../src
make
cd ../../
```

2. Set PYTHONPATH and JITQPATH
```
export JITQPATH=$PWD
export PYTHONPATH=$PWD/python
```

3. Run tests and format checkers before commit<br>
```
tools/check_all.sh
python3 -m unittest discover -v -s python/jitq/tests
cd backend/build
ctest -V
```
## Common Issues
1. libruntime.so cannot be found by python cffi

Add $JITQPATH/backend/build to the environment variable LD_LIBRARY_PATH.