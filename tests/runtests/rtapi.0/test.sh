#!/bin/sh
set -e

CURRENT_PATH="$(dirname $(readlink -f $0))"
realtime stop
set +e
${CURRENT_PATH}/test_runtime_module_init
if [ $? -ne 1 ]; then
    echo "rtapi_test: expected 1, got " $?
    exit 1;
fi
set -e
realtime start
${CURRENT_PATH}/test_runtime_module_init
if [ $? -ne 0 ]; then
    echo "rtapi_test: expected 0, got " $?
    exit 1;
fi

realtime stop
exit 0
