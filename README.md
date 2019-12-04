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

Uninstall CMake if necessary:

```bash
sudo apt remove cmake
```

Then:

```bash
sudo mkdir /opt/cmake-3.16.0/ && \
cd /opt/cmake-3.16.0/ && \
wget https://github.com/Kitware/CMake/releases/download/v3.16.0/cmake-3.16.0-Linux-x86_64.tar.gz -O - \
    | sudo tar -xz --strip-components=1 && \
for file in bin/*; \
do \
    sudo ln -s $PWD/$file /usr/bin/$(basename $file); \
done
```

4. Clang + LLVM

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

Make the following command executed in the shells you use for development,
for example via `~/.bashrc` or `/etc/profile.d/cmake-config.sh`:

```bash
export CMAKE_PREFIX_PATH=$CMAKE_PREFIX_PATH:/opt/clang+llvm-7.0.1
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
CXX=clang++-7.0 CC=clang-7.0 cmake -G Ninja ../ \
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
wget https://github.com/danmar/cppcheck/archive/1.88.zip && \
unzip 1.88.zip && \
cd /tmp/cppcheck-1.88 && \
make -j$(nproc) SRCDIR=build CFGDIR=/opt/cppcheck-1.88/share/cfg HAVE_RULES=yes \
     CXXFLAGS="-O2 -DNDEBUG -Wall -Wno-sign-compare -Wno-unused-function" && \
sudo mkdir -p /opt/cppcheck-1.88/bin /opt/cppcheck-1.88/share && \
sudo cp -r cfg /opt/cppcheck-1.88/share && \
sudo cp cppcheck /opt/cppcheck-1.88/bin/cppcheck-1.88;
```

Add links:

```bash
for bin in /opt/cppcheck-1.88/bin/cppcheck-1.88; do \
    sudo ln -s $bin /usr/bin/; \
done
```

7. GraphViz

```bash
sudo apt-get install libgraphviz-dev
```

8. Boost

```bash
sudo apt install python3-dev autotools-dev libicu-dev libbz2-dev
cd /tmp/ && \
wget https://dl.bintray.com/boostorg/release/1.71.0/source/boost_1_71_0.tar.gz -O - \
    | tar -xz && \
cd /tmp/boost_1_71_0 && \
echo "using clang : 7.0 : $(which clang-7.0) ; " > tools/build/src/user-config.jam && \
PYTHONVERSION="$(python3 -c "import sys; print(str(sys.version_info.major) + '.' + str(sys.version_info.minor))")" && \
./bootstrap.sh --with-python=$(which python3) && \
./bjam --toolset=clang-7.0 --python=$PYTHONVERSION -j$(nproc) --prefix=/opt/boost-1.71.0 && \
sudo ./bjam install --prefix=/opt/boost-1.71.0
```

Make the following command executed in the shells you use for development,
for example via `~/.bashrc` or `/etc/profile.d/cmake-config.sh`:

```bash
export CMAKE_PREFIX_PATH=$CMAKE_PREFIX_PATH:/opt/boost-1.71.0
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
pip3 install wheel && \
mkdir -p /tmp/arrow/cpp/build && \
cd /tmp/arrow/cpp/build && \
CXX=clang++-7.0 CC=clang-7.0 \
    cmake \
        -DCMAKE_CXX_STANDARD=17 \
        -DCMAKE_INSTALL_PREFIX=/tmp/arrow/dist \
        -DCMAKE_INSTALL_LIBDIR=lib \
        -DCMAKE_INSTALL_RPATH_USE_LINK_PATH=ON \
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
make -j$(nproc) install && \
cd /tmp/arrow/python && \
PYARROW_WITH_PARQUET=1 ARROW_HOME=/tmp/arrow/dist \
    python3 setup.py build_ext --bundle-arrow-cpp bdist_wheel && \
sudo mkdir -p /opt/arrow-0.14/share && \
sudo cp /tmp/arrow/python/dist/*.whl /opt/arrow-0.14/share &&\
sudo cp -r /tmp/arrow/dist/* /opt/arrow-0.14/ && \
pip3 install /opt/arrow-0.14/share/*.whl
```

Make the following command executed in the shells you use for development,
for example via `~/.bashrc` or `/etc/profile.d/cmake-config.sh`:

```bash
export CMAKE_PREFIX_PATH=$CMAKE_PREFIX_PATH:/opt/arrow-0.14
```

### AWS SDK (optional)

```bash
sudo apt install libcurl4-openssl-dev libssl-dev \
                 uuid-dev zlib1g-dev libpulse-dev && \
mkdir -p /tmp/aws-sdk-cpp && \
cd /tmp/aws-sdk-cpp && \
wget https://github.com/aws/aws-sdk-cpp/archive/1.7.138.tar.gz -O - \
    | tar -xz --strip-components=1 && \
mkdir -p /tmp/aws-sdk-cpp/build && \
cd /tmp/aws-sdk-cpp/build && \
CXX=clang++-7.0 CC=clang-7.0 \
    sudo cmake \
        -DBUILD_ONLY="dynamodb;lambda;s3;sqs" \
        -DCMAKE_BUILD_TYPE=Debug \
        -DCPP_STANDARD=17 \
        -DENABLE_TESTING=OFF \
        -DCUSTOM_MEMORY_MANAGEMENT=OFF \
        -DCMAKE_INSTALL_PREFIX=/opt/aws-sdk-cpp-1.7/ \
        -DAWS_DEPS_INSTALL_DIR:STRING=/opt/aws-sdk-cpp-1.7/ \
        .. && \
sudo make -j$(nproc) install
```

Make the following command executed in the shells you use for development,
for example via `~/.bashrc` or `/etc/profile.d/cmake-config.sh`:

```bash
export CMAKE_PREFIX_PATH=$CMAKE_PREFIX_PATH:/opt/aws-sdk-cpp-1.7
```

## Development

1. Set `PYTHONPATH` and `JITQPATH` (in every shell):

```bash
# Run in the root folder of the project
export JITQPATH=$PWD
export PYTHONPATH=$JITQPATH/python:$JITQPATH/backend/build
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/opt/clang+llvm-7.0.1/lib
```

2. Setup build (once):

Configure CMake:

```bash
cd $JITQPATH/backend/build
CXX=clang++-7.0 CC=clang-7.0 cmake ../src/
```

Configure JIT compilation:

```bash
echo -e "CC=clang-7.0\nCXX=clang++-7.0\nLIBOMPDIR=/opt/clang+llvm-7.0.1/lib" > ../src/generate/src/code_gen/Makefile.local
```

3. Build (after every change to the backend):

```bash
cd $JITQPATH/backend/build
make -j$(nproc)
```

4. Run tests and format checkers before commit:

```bash
$JITQPATH/tools/check_all.sh
cd $JITQPATH/python/jitq/tests && python3 -u -m pytest -vs
cd $JITQPATH/backend/build && make test
```

5. Run unittest that need AWS infrastructure (optional):

Install [docker](https://docs.docker.com/install/) and [docker-compose](https://docs.docker.com/compose/install/).
In a new terminal, run:

```bash
cd $JITQPATH/tools/aws
docker-compose up
```

In your usual terminal, set up the credentials of the test infrastructure:

```bash
export AWS_ACCESS_KEY_ID=xxx
export AWS_SECRET_ACCESS_KEY=xxxxxxxx
export AWS_S3_ENDPOINT=http://localhost:9000
```

Run the tests that need AWS, for example:

```bash
$JITQPATH/python/jitq/tests/test_cpp_backend.py -v TestParquetS3
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
