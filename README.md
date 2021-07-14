## JITQ - Just-In-Time Query Compilation

[![pipeline status](https://gitlab.inf.ethz.ch/OU-SYSTEMS/jitq/badges/master/pipeline.svg)](https://gitlab.inf.ethz.ch/OU-SYSTEMS/jitq/-/commits/master)
[![coverage report](https://gitlab.inf.ethz.ch/OU-SYSTEMS/jitq/badges/master/coverage.svg)](https://gitlab.inf.ethz.ch/OU-SYSTEMS/jitq/-/commits/master)

JITQ is a dataflow framework featuring just-in-time compilation and query optimization in presence of user-defined functions.

## Installation

This is a best-effort guide to help getting started with JITQ development.
Since it may run out-of-date, you may also look into [`.gitlab-ci.yml`](.gitlab-ci.yml),
which contains the commands used to set up and run compilation and tests for every commit,
as well as the [Dockerfile](https://hub.docker.com/r/ingomuellernet/buildenv/dockerfile/)
which is used to run the CI tests.

1. `JITQPATH`

For the installation and during development, make sure the following variable is set (from inside the git repository):

```bash
export JITQPATH=$(git rev-parse --show-toplevel)
```

2. Git submodules

```bash
cd $JITQPATH
git submodule update --recursive --init
```

3. Python requirements

Install necessary system packages.

```bash
sudo apt install python3 python3-pip python3-dev python3-setuptools
```

Create a virtualenv and activate it:

```bash
python3 -m venv <name_of_the_virtual_environment>
source <name_of_the_virtual_environment>/bin/activate
```

Install the requirements:

```bash
pip3 install -r $JITQPATH/requirements.txt
```

4. Dev requirements

```bash
sudo apt install build-essential \
        bison \
        flex \
        gcc-7 \
        g++-7 \
        libtinfo5 \
        libtinfo-dev \
        pkg-config \
        wget \
        xz-utils
```

5. CMake

Uninstall CMake if necessary:

```bash
sudo apt remove cmake
```

Then:

```bash
sudo mkdir /opt/cmake-3.21.0/ && \
cd /opt/cmake-3.21.0/ && \
wget https://github.com/Kitware/CMake/releases/download/v3.21.0/cmake-3.21.0-linux-x86_64.tar.gz -O - \
    | sudo tar -xz --strip-components=1 && \
for file in bin/*; \
do \
    sudo ln -s $PWD/$file /usr/bin/$(basename $file); \
done
```

6. Clang + LLVM

Download and add links:

```bash
sudo mkdir /opt/clang+llvm-11.1.0/ && \
cd /opt/clang+llvm-11.1.0/ && \
    wget --progress=dot:giga https://github.com/llvm/llvm-project/releases/download/llvmorg-11.1.0/clang+llvm-11.1.0-x86_64-linux-gnu-ubuntu-16.04.tar.xz -O - \
     | sudo tar -x -I xz --strip-components=1 && \
for file in bin/*; \
do \
    sudo ln -s $PWD/$file /usr/bin/$(basename $file)-11.1; \
done
```

Make the following command executed in the shells you use for development,
for example via `~/.bashrc` or `/etc/profile.d/cmake-config.sh`:

```bash
export CMAKE_PREFIX_PATH=$CMAKE_PREFIX_PATH:/opt/clang+llvm-11.1.0
```

8. cppcheck

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
make -j$(nproc) CFGDIR=/opt/cppcheck-1.88/share/cfg HAVE_RULES=yes \
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

9. GraphViz

```bash
sudo apt install libgraphviz-dev
```

10. Boost

```bash
sudo apt install python3-dev autotools-dev libicu-dev libbz2-dev
cd /tmp/ && \
wget https://boostorg.jfrog.io/artifactory/main/release/1.76.0/source/boost_1_76_0.tar.gz -O - \
    | tar -xz && \
cd /tmp/boost_1_76_0 && \
echo "using clang : 11.1 : $(which clang-11.1) ; " > tools/build/src/user-config.jam && \
PYTHONVERSION="$(python3 -c "import sys; print(str(sys.version_info.major) + '.' + str(sys.version_info.minor))")" && \
export CPLUS_INCLUDE_PATH="$CPLUS_INCLUDE_PATH:$(python3 -c 'import sysconfig as s; print(s.get_path("include"))')" && \
./bootstrap.sh --with-python=$(which python3) --prefix=/opt/boost-1.76.0 && \
./b2 numa=on define=BOOST_FIBERS_SPINLOCK_TTAS_ADAPTIVE_FUTEX --toolset=clang-11.1 cxxflags=-fPIC --python=$PYTHONVERSION -j$(nproc) --prefix=/opt/boost-1.76.0 && \
sudo ./b2 numa=on install --prefix=/opt/boost-1.76.0
```

Make the following command executed in the shells you use for development,
for example via `~/.bashrc` or `/etc/profile.d/cmake-config.sh`:

```bash
export CMAKE_PREFIX_PATH=$CMAKE_PREFIX_PATH:/opt/boost-1.76.0
```

11. Apache Arrow

Run the following outside your `venv` (but with the same version of Python).

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
CXX=clang++-11.1 CC=clang-11.1 \
    cmake \
        -DPYTHON_EXECUTABLE=$(which python3) \
        -DCMAKE_CXX_STANDARD=17 \
        -DCMAKE_INSTALL_PREFIX=/opt/arrow-0.14/ \
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
make -j$(nproc) && \
sudo make install && \
cd /tmp/arrow/python && \
PYARROW_WITH_PARQUET=1 ARROW_HOME=/opt/arrow-0.14/ \
    python3 setup.py build_ext --bundle-arrow-cpp bdist_wheel && \
sudo mkdir -p /opt/arrow-0.14/share && \
sudo cp /tmp/arrow/python/dist/*.whl /opt/arrow-0.14/share
```

Make the following command executed in the shells you use for development,
for example via `~/.bashrc` or `/etc/profile.d/cmake-config.sh`:

```bash
export CMAKE_PREFIX_PATH=$CMAKE_PREFIX_PATH:/opt/arrow-0.14
```

Finally, run the following **in your `venv`**.

```bash
pip3 install /opt/arrow-0.14/share/*.whl
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
CXX=clang++-11.1 CC=clang-11.1 \
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
export JITQPATH=$(git rev-parse --show-toplevel)
export PYTHONPATH=$JITQPATH/python:$JITQPATH/backend/build
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/opt/clang+llvm-11.1.0/lib
```

2. Setup build (once):

Configure CMake:

```bash
cd $JITQPATH/backend/build
CXX=clang++-11.1 CC=clang-11.1 cmake ../src/ -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
```

Configure JIT compilation:

```bash
echo -e "CC=clang-11.1\nCXX=clang++-11.1\nCXXFORMAT=clang-format-11.1\nLIBOMPDIR=/opt/clang+llvm-11.1.0/lib" \
    > $JITQPATH/backend/src/generate/src/code_gen/Makefile.local
```

3. Build (after every change to the backend):

```bash
cd $JITQPATH/backend/build
make -j$(nproc)
```

4. Run tests and format checkers before commit:

```bash
$JITQPATH/tools/check_all.sh
cd $JITQPATH/python/jitq/tests && python3 -u -m pytest -vs --filesystem file
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
cmake <...> -DWITH_LLVM_ASAN=ON
```

Then, set up the following environment variables:

```bash
export ASAN_SYMBOLIZER_PATH=/opt/clang+llvm-11.1.0/bin/llvm-symbolizer
export ASAN_LIBRARY_PATH=/opt/clang+llvm-11.1.0/lib/clang/11.1.0/lib/linux/libclang_rt.asan-x86_64.so
```

Finally, run unittests or queries you want to test.
Any executable compiled by CMake will work out of the box.
Python-based tests must be run through a script, e.g.:

```bash
cd $JITQPATH/python
../tools/run_with_asan.sh python3 -u -m unittest discover -v -s jitq/tests
```

## Use serverless runners

You can run the serverless runners in three different modes:

* 'process': Runs each worker in a local Python process (default)
* 'sam': Runs each worker in a local Docker container using the official runtime
* 'aws': Runs the workers in the AWS cloud

### Using new local Python processes instead of lambdas

#### Prerequisites

Start local endpoints:

```bash
cd $JITQPATH/tools/aws
docker-compose up
```

#### Development cycle

In every console you want to run tests, set up the correct environment variables. For example:

```bash
source $JITQPATH/tools/aws/set-env.sh
cd $JITQPATH/python/jitq/
pytest --serverless_stack_type process  # or select a specific test
```

### AWS SAM (local)

#### Prerequesites

Make sure you have Python 3.7.

Install SAM:

```bash
pip install -r $JITQPATH:/tools/aws/requirements.txt
```

Set up work-around to include `*.so` in SAM packages (assuming you are running in a venv), see [this issue](https://github.com/awslabs/aws-lambda-builders/issues/115):

```bash
cp $JITQPATH/tools/aws/_patch_aws_lambda_builders.* $(python -c "import sysconfig; print(sysconfig.get_paths()['purelib'])")
```

This only works if `python -c "import site; print(site.ENABLE_USER_SITE)"` returns `True` in your venv. If it doesn't, [do this](https://stackoverflow.com/a/40972692/651937).

Start the local endpoints like above.

#### Development cycle

Build JITQ using the official Docker image as compilation environment:

```bash
cd $JITQPATH/python/jitq/serverless
make
```

Configure the environment variables as above. Then build the SAM package and spawn the local lambda endpoint:

```bash
source $JITQPATH/tools/aws/set-env.sh
cd $JITQPATH/python/jitq/serverless
sam build
sam local start-lambda --env-vars $JITQPATH/tools/aws/test_environment.json --host $(hostname --fqdn)
```

Run tests against the local endpoint:

```bash
source $JITQPATH/tools/aws/set-env.sh
cd $JITQPATH/python/jitq/
pytest --serverless_stack_type sam  # or select a specific test
```

### AWS Lambda (cloud)

Build the package as above, then deploy like this:

```bash
source $JITQPATH/tools/aws/unset-env.sh
cd $JITQPATH/python/jitq/serverless
sam build
sam deploy \
    --s3-bucket <EXISTING_BUCKET> \
    --stack-name <YOUR_STACK_NAME> \
    --capabilities CAPABILITY_IAM
```

And run like this:

```bash
source $JITQPATH/tools/aws/unset-env.sh
cd $JITQPATH/python/jitq/
pytest --serverless_stack_type aws --s3_bucket_name <BUCKET_FOR_TESTDATA> # or select a specific test
```

Before the serverless runners can access data from a bucket on S3, you need to give them access to that bucket.
This is in particular true for the bucket you specify via `--s3_bucket_name` for the unit tests.
The most fine-granular way to do this is via [this guide](https://aws.amazon.com/premiumsupport/knowledge-center/lambda-execution-role-s3-bucket/)
(except you should use the existing role of the lambda function,
which is part of the [stack](console.aws.amazon.com/cloudformation/) that is created during deployment, instead of creating one).

## Common Issues

1. `libruntime.so` cannot be found by python cffi.

Add `$JITQPATH/backend/build` to the environment variable `LD_LIBRARY_PATH`.
