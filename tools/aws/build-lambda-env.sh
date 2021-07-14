#!/usr/bin/env bash

ls /opt/arrow-*/lib/lib{arrow,parquet}.so* \
   /opt/aws-sdk-cpp-*/lib64/libaws-c* \
   /opt/boost-1.*/lib/libboost_*so* \
   /opt/clang+llvm-*/lib/libomp.so* \
   /opt/arrow-*/share/pyarrow-*.whl | \
    while read line
    do
        # Copy to what ends up at /opt/<tool>/lib
        mkdir -p /output/$(dirname $line)
        cp -d $line /output/$line

        # Link that ends up at /opt/lib
        linkname="/opt/${line#/opt/*/}"
        linktarget="$(realpath --relative-to="/opt/lib" "$line")"
        mkdir -p /output/$(dirname $linkname)
        ln -fs $linktarget /output/$linkname
    done

cd /output/build && \
CC=clang-11.1 CXX=clang++-11.1 \
    cmake /jitq/backend/src \
        -DCMAKE_BUILD_TYPE=Release \
        && \
make -j$(nproc) runner &&
find . -maxdepth 1 -executable -type f | \
    while read line
    do
        cp $line /output/lib
    done
