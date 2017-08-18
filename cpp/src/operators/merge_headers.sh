#!/usr/bin/env bash
HEADER=Operators.h
echo "#ifndef __ALL_OPERATORS_HEADERS__" > ${HEADER}
echo "#define __ALL_OPERATORS_HEADERS__" >> ${HEADER}
echo "" >> ${HEADER}
for file in *.h
do
    if [ ${file} != ${HEADER} ]
    then
    echo "#include \"$file\"" >> ${HEADER}
    fi
done
echo "" >> ${HEADER}
echo "#endif" >> ${HEADER}