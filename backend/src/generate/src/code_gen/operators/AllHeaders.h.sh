#!/usr/bin/env bash

HEADER=$1
shift 2

(
    echo "#ifndef CODE_GEN_OPERATORS_ALLHEADERS_H"
    echo "#define CODE_GEN_OPERATORS_ALLHEADERS_H"
    echo ""
    for file in "$@"
    do
        echo "#include \"$file\""
    done
    echo ""
    echo "#endif"
) > ${HEADER}
