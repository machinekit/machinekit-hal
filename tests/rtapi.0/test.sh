#!/bin/sh
rm -f rtapi_test
set -e
if [ -z "$MACHINEKIT_LIB" ] || [ -z "$MACHINEKIT_INCLUDE" ]; then
   INCLUDE=../../include
   LIB=../../lib
else
   INCLUDE=${MACHINEKIT_INCLUDE}
   LIB=${MACHINEKIT_LIB}
fi
gcc -g -DULAPI \
    -I${INCLUDE} \
    rtapi_test.c \
    ${LIB}/libmtalk.so \
    ${LIB}/libmkulapi.so \
    ${LIB}/libmkhal.so \
    -o rtapi_test

realtime stop
set +e
./rtapi_test
if [ $? -ne 1 ]; then
    echo "rtapi_test: expected 1, got " $?
    exit 1;
fi
set -e
realtime start
./rtapi_test
if [ $? -ne 0 ]; then
    echo "rtapi_test: expected 0, got " $?
    exit 1;
fi

realtime stop
exit 0
