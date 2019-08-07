#!/usr/bin/env bash

HEADER=$1
shift 2

(
    echo "#ifndef CODE_GEN_CPP_OPERATORS_OPERATORS_H"
    echo "#define CODE_GEN_CPP_OPERATORS_OPERATORS_H"
    echo ""
    for file in "$@"
    do
        echo "#include \"$file\""
    done
    echo ""
    echo "#endif"
) > ${HEADER}
