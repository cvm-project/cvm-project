#!/usr/bin/env bash

ROOT_PATH="$(cd $(dirname $0) ; pwd -P | xargs dirname)"

# Combined exit code: non-zero if any checker returns non-zero
rc=0

function run_checker {
  cmd="$@"
  echo "Running '$@'..."
  $cmd

  rc="$([ $rc -eq 0 -a $? -eq 0 ]; echo $?)"
}

run_checker "${ROOT_PATH}"/tools/check_format_python.sh
run_checker "${ROOT_PATH}"/tools/check_pylint.sh
run_checker make -C "${ROOT_PATH}"/backend/build/ check-format
run_checker make -C "${ROOT_PATH}"/backend/build/ cppcheck

exit $rc
