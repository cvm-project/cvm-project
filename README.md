## JITQ - Just-In-Time Query Compilation

JITQ is a dataflow framework featuring just-in-time compilation and query optimization in presence of user-defined functions.

## Installation

This is a best-effort guide to help getting started with JITQ development.
Since it may run out-of-date, you may also look into [`.gitlab-ci.yml`](.gitlab-ci.yml),
which contains the commands used to set up and run compilation and tests for every commit,
as well as the [Dockerfile](https://hub.docker.com/r/ingomuellernet/buildenv/dockerfile/)
which is used to run the CI tests.

0. Git submodules

```bash
git submodule update --recursive --init
```

1. Python requirements

Install necessary system packages.

```bash
sudo apt install python3 python3-pip python3-dev
```

Create a virtualenv and activate it
```bash
python3 -m venv <name_of_the_virtual_environment>
source <name_of_the_virtual_environment>/bin/activate
```

Install the requirements
```bash
pip3 install -r requirements.txt
```

2. Dev requirements

```bash
sudo apt install build-essential \
        libtinfo-dev \
        bison \
        flex \
        gcc-7 \
        g++-7 \
        pkg-config \
        wget \
        xz-utils
```

3. CMake

```bash
sudo mkdir /opt/cmake-3.13.4/ && \
cd /opt/cmake-3.13.4/ && \
wget https://cmake.org/files/v3.13/cmake-3.13.4-Linux-x86_64.tar.gz -O - \
    | sudo tar -xz --strip-components=1 && \
for file in bin/*; \
do \
    sudo ln -s $PWD/$file /usr/bin/$(basename $file)-3.13; \
done
```

4. Clang

Download and add links:

```bash
sudo mkdir /opt/clang+llvm-7.0.1/ && \
cd /opt/clang+llvm-7.0.1/ && \
wget http://releases.llvm.org/7.0.1/clang+llvm-7.0.1-x86_64-linux-gnu-ubuntu-16.04.tar.xz -O - \
     | sudo tar -x -I xz --strip-components=1 && \
for file in bin/*; \
do \
    sudo ln -s $PWD/$file /usr/bin/$(basename $file)-7.0; \
done
```

5. Gold linker

```bash
sudo apt install binutils-dev ninja-build python-minimal
```

Download and build with ninja:

```bash
cd /tmp/ && \
wget http://releases.llvm.org/7.0.1/llvm-7.0.1.src.tar.xz && \
tar -xf llvm-7.0.1.src.tar.xz && \
mkdir /tmp/llvm-7.0.1.src/build && \
cd /tmp/llvm-7.0.1.src/build && \
CXX=clang++-7.0 CC=clang-7.0 cmake-3.13 -G Ninja ../ \
    -DLLVM_BINUTILS_INCDIR=/usr/include \
    -DLLVM_TARGETS_TO_BUILD=X86 \
    -DCMAKE_BUILD_TYPE=MinSizeRel \
    -DLLVM_OPTIMIZED_TABLEGEN=ON && \
ninja LLVMgold
```

Copy goldlinker:

```bash
sudo cp /tmp/llvm-7.0.1.src/build/lib/LLVMgold.so /opt/clang+llvm-7.0.1/lib
```

6. cppcheck

```bash
sudo apt install build-essential \
        libpcre3-dev \
        unzip
```

Download and build:

```bash
cd /tmp/ && \
wget https://github.com/danmar/cppcheck/archive/1.81.zip && \
unzip 1.81.zip && \
cd /tmp/cppcheck-1.81 && \
make SRCDIR=build CFGDIR=/opt/cppcheck-1.81/share/cfg HAVE_RULES=yes \
     CXXFLAGS="-O2 -DNDEBUG -Wall -Wno-sign-compare -Wno-unused-function" && \
sudo mkdir -p /opt/cppcheck-1.81/bin /opt/cppcheck-1.81/share && \
sudo cp -r cfg /opt/cppcheck-1.81/share && \
sudo cp cppcheck /opt/cppcheck-1.81/bin/cppcheck-1.81;
```

Add links:

```bash
for bin in /opt/cppcheck-1.81/bin/cppcheck-1.81; do \
    sudo ln -s $bin /usr/bin/; \
done
```

7. GraphViz

```bash
sudo apt-get install libgraphviz-dev
```

8. Boost

```bash
sudo apt install python-dev autotools-dev libicu-dev libbz2-dev
cd /tmp/ && \
wget https://dl.bintray.com/boostorg/release/1.69.0/source/boost_1_69_0.tar.gz -O - \
    | tar -xz && \
cd /tmp/boost_1_69_0 && \
echo "using clang : 7.0 : $(which clang-7.0) ; " > tools/build/src/user-config.jam && \
PYTHONVERSION="$(python3 -c "import sys; print(str(sys.version_info.major) + '.' + str(sys.version_info.minor))")" && \
./bootstrap.sh && \
./bjam --toolset=clang-7.0 --python=$PYTHONVERSION -j6 --prefix=/opt/boost-1.69.0 && \
sudo ./bjam install
```

9. Apache Arrow

(Note: The patch fixes a problem mentioned [here](https://issues.apache.org/jira/browse/ARROW-5960). Maybe it will be addressed in a future verison.)

```bash
mkdir -p /tmp/arrow && \
cd /tmp/arrow && \
wget https://github.com/apache/arrow/archive/apache-arrow-0.14.0.tar.gz -O - \
    | tar -xz --strip-components=1 && \
{ patch -p1  <<\ENDOFMESSAGE
    diff -pur a/cpp/CMakeLists.txt b/cpp/CMakeLists.txt
    --- a/cpp/CMakeLists.txt   2019-06-29 00:26:37.000000000 +0200
    +++ b/cpp/CMakeLists.txt    2019-07-16 16:36:03.980153919 +0200
    @@ -642,8 +642,8 @@ if(ARROW_STATIC_LINK_LIBS)
       add_dependencies(arrow_dependencies ${ARROW_STATIC_LINK_LIBS})
     endif()
    
    -set(ARROW_SHARED_PRIVATE_LINK_LIBS ${ARROW_STATIC_LINK_LIBS} ${BOOST_SYSTEM_LIBRARY}
    -                                   ${BOOST_FILESYSTEM_LIBRARY} ${BOOST_REGEX_LIBRARY})
    +set(ARROW_SHARED_PRIVATE_LINK_LIBS ${ARROW_STATIC_LINK_LIBS} ${BOOST_FILESYSTEM_LIBRARY}
    +                                   ${BOOST_SYSTEM_LIBRARY} ${BOOST_REGEX_LIBRARY})
    
     list(APPEND ARROW_STATIC_LINK_LIBS ${BOOST_SYSTEM_LIBRARY} ${BOOST_FILESYSTEM_LIBRARY}
                 ${BOOST_REGEX_LIBRARY})
ENDOFMESSAGE
} && \
pip3 install -r /tmp/arrow/python/requirements-build.txt && \
mkdir -p /tmp/arrow/cpp/build && \
cd /tmp/arrow/cpp/build && \
CXX=clang++-7.0 CC=clang-7.0 \
    cmake \
        -DCMAKE_CXX_STANDARD=17 \
        -DCMAKE_INSTALL_PREFIX=/tmp/arrow/dist \
        -DCMAKE_INSTALL_LIBDIR=lib \
        -DARROW_WITH_RAPIDJSON=ON \
        -DARROW_PARQUET=ON \
        -DARROW_PYTHON=ON \
        -DARROW_FLIGHT=OFF \
        -DARROW_GANDIVA=OFF \
        -DARROW_BUILD_UTILITIES=OFF \
        -DARROW_CUDA=OFF \
        -DARROW_ORC=OFF \
        -DARROW_JNI=OFF \
        -DARROW_TENSORFLOW=OFF \
        -DARROW_HDFS=OFF \
        -DARROW_BUILD_TESTS=OFF \
        -DARROW_RPATH_ORIGIN=ON \
        .. && \
make install && \
cd /tmp/arrow/python && \
PYARROW_WITH_PARQUET=1 ARROW_HOME=/tmp/arrow/dist \
    python3 setup.py build_ext --bundle-arrow-cpp bdist_wheel && \
sudo mkdir -p /opt/arrow-0.14/share && \
sudo cp /tmp/arrow/python/dist/*.whl /opt/arrow-*/share &&\
sudo cp -r /tmp/arrow/dist/* /opt/arrow-*/ && \
pip3 install /opt/arrow-*/share/*.whl
```

## Development

0. Setup build:

Configure CMake:

```bash
cd jitq/backend/build
CXX=clang++-7.0 CC=clang-7.0 cmake-3.13 ../src/ -DLLVM_DIR=/opt/clang+llvm-7.0.1/lib/cmake/llvm -DBOOSTROOT=/opt/boost-1.69.0 -Darrow_DIR=/opt/arrow-0.14/lib/cmake/arrow
```

Configure JIT compilation:

```bash
echo -e "CC=clang-7.0\nCXX=clang++-7.0\nLIBOMPDIR=/opt/clang+llvm-7.0.1/lib" > ../src/code_gen/cpp/Makefile.local
```

1. Build:

```bash
make
```

2. Set `PYTHONPATH` and `JITQPATH`:

```bash
export JITQPATH=$PWD
export PYTHONPATH=$JITQPATH/python:$JITQPATH/backend/build
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/opt/clang+llvm-7.0.1/lib
```

3. Run tests and format checkers before commit:

```bash
tools/check_all.sh
python3 -m unittest discover -v -s python/jitq/tests
cd backend/build
make test
(cd ../../python/jitq/tests/ && python3 -m unittest discover -v)
```

## Compile and run with sanitizers

You can compile and run JITQ with "sanitizers", which do extra checks during runtime.
Currently, Clang's [LeakSanitizer](https://github.com/google/sanitizers/wiki/AddressSanitizerLeakSanitizer) (LSan),
[AddressSanitizer](https://github.com/google/sanitizers/wiki/AddressSanitizer) (ASan),
and [UndefinedBehavioSanitizer](https://clang.llvm.org/docs/UndefinedBehaviorSanitizer.html) (UBSan)
are pre-setup.

To compile, add the following flag to `cmake` and recompile:

```bash
cmake <...> -DLLVM_ASAN=ON
```

Then, set up the following environment variables:

```bash
export ASAN_SYMBOLIZER_PATH=/opt/clang+llvm-7.0.1/bin/llvm-symbolizer
export ASAN_LIBRARY_PATH=/opt/clang+llvm-7.0.1/lib/clang/7.0.1/lib/linux/libclang_rt.asan-x86_64.so
```

Finally, run unittests or queries you want to test.
Any executable compiled by CMake will work out of the box.
Python-based tests must be run through a script, e.g.:

```bash
cd $JITQPATH/python
../tools/run_with_asan.sh python3 -u -m unittest discover -v -s jitq/tests
```

## Common Issues

1. `libruntime.so` cannot be found by python cffi.

Add `$JITQPATH/backend/build` to the environment variable `LD_LIBRARY_PATH`.
