#!/bin/bash

OUTPUT_PATH="$(pwd -P)/coverage"

input_files=("$@")
input_flags=$(for file in ${input_files[*]}; do echo --add-tracefile "$(readlink -f "$file")"; done)

echo "Merging coverage files: ${input_files[*]}"

mkdir -p "$OUTPUT_PATH"

gcovr $input_flags \
    --print-summary \
    --xml -o "$OUTPUT_PATH/coverage.xml"

cd "$JITQPATH/backend/src"
gcovr $input_flags \
    --html-details -o "$OUTPUT_PATH/coverage.html"
