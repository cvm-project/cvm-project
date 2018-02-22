#!/usr/bin/env bash

ROOT_PATH="$(cd $(dirname $0) ; pwd -P | xargs dirname)"

if [ $# -eq 0 ]
  then
    # run from root
    echo "No argument passed, running on all python files"
    autopep8 -a -i -r "${ROOT_PATH}"/jitq "${ROOT_PATH}"/ml
  else
  # run on files
  autopep8 -a -i "$@"
fi
