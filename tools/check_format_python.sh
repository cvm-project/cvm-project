#!/usr/bin/env bash

ROOT_PATH="$(cd $(dirname $0) ; pwd -P | xargs dirname)"

if [ $# -eq 0 ]
  then
    # Run from root
    echo "No argument passed, running on all python files"
    echo autopep8 -d -r "${ROOT_PATH}"/python
    output=$(autopep8 -a -d -r "${ROOT_PATH}"/python)
  else
    # Run on files
    output=$(autopep8 -a -d "$@")
fi

# Print output to stdout and fail script if there was one (i.e., if autopep8
# suggested a change)
echo "$output"
[[ -z "$output" ]]
