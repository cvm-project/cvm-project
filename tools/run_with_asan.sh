#!/bin/bash

ROOT_PATH="$(cd $(dirname $0) ; pwd -P | xargs dirname)"

export ASAN_OPTIONS="$ASAN_OPTIONS:suppressions=$ROOT_PATH/backend/asan.supp:symbolize=1"
export LSAN_OPTIONS="$LSAN_OPTIONS:suppressions=$ROOT_PATH/backend/lsan.supp:symbolize=1"
export UBSAN_OPTIONS="$UBSAN_OPTIONS:suppressions=$ROOT_PATH/backend/ubsan.supp:print_stacktrace=1"
export LD_PRELOAD="$ASAN_LIBRARY_PATH $ROOT_PATH/backend/build/libdlclose_noop.so"

"$@"
