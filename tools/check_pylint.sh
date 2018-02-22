#!/usr/bin/env bash

ROOT_PATH="$(cd $(dirname $0) ; pwd -P | xargs dirname)"

if [ $# -eq 0 ]
  then
    # Run from root
    echo "No argument passed, running on all python files"
    find \
        "${ROOT_PATH}"/jitq \
        "${ROOT_PATH}"/ml \
      -name "*.py" | xargs pylint -f colorized --rcfile="${ROOT_PATH}"/.pylintrc 
  else
    # Run on files
    pylint -f colorized --rcfile="${ROOT_PATH}"/.pylintrc "$@"
fi
