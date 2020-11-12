#!/usr/bin/env bash

ROOT_PATH="$(cd $(dirname $0) ; pwd -P | xargs dirname)"

if [ $# -eq 0 ]
  then
    # Run from root
    echo "No argument passed, running on all python files"
    cd "$ROOT_PATH"
    git ls-files **/*.py \
      | xargs pylint -f colorized --rcfile="${ROOT_PATH}"/.pylintrc
  else
    # Run on files
    pylint -f colorized --rcfile="${ROOT_PATH}"/.pylintrc "$@"
fi
