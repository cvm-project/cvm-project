#!/usr/bin/env bash

SCRIPTPATH=$(cd $(dirname $0) ; pwd -P)

HEADER=$1

(
    echo "#ifndef OPERATORS_OPERATORS_H_"
    echo "#define OPERATORS_OPERATORS_H_"
    echo ""
    for file in "$SCRIPTPATH"/*.h
    do
        if [ ${file} == ${HEADER} ]; then continue; fi
        echo "#include \"$file\""
    done
    echo ""
    echo "#endif"
) > ${HEADER}
