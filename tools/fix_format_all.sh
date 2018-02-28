#!/usr/bin/env bash

ROOT_PATH="$(cd $(dirname $0) ; pwd -P | xargs dirname)"

"${ROOT_PATH}"/tools/fix_format_python.sh
make -C "${ROOT_PATH}"/backend/build/ fix-format
