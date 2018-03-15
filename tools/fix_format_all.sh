#!/usr/bin/env bash

ROOT_PATH="$(cd $(dirname $0) ; pwd -P | xargs dirname)"

"${ROOT_PATH}"/tools/autoformat_python.sh
make -C "${ROOT_PATH}"/cpp/build/ fix-format