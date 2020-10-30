#!/bin/bash

OUTPUT_PATH="$(pwd -P)"

output_filename="$1"

if [[ -z "$output_filename" ]]
then
    output_filename="coverage.json"
fi

cd "$JITQPATH/backend/src"

gcovr \
    --gcov-executable "$JITQPATH/tools/llvm-cov.sh" \
    . "$JITQPATH/backend/build/" \
    --json | (cd "$OUTPUT_PATH"; cat - > "$output_filename")

