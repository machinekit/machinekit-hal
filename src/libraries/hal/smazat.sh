#!/bin/env bash

if (($# < 2)); then
    echo "Script $0 takes two arguments!"
    exit 10
fi

readarray -td\; a <<<"${1}"

GLOBAL_SYMBOLS=""

for i in ${a[@]}; do
    T="$(objcopy -j .rtapi_export -O binary ${i} /dev/stdout | tr -s '\0' | xargs -r0 printf '%s;\n' | grep .)"
    GLOBAL_SYMBOLS+="$T"
done

F=$(printf "%b\n" \
    "{ global :" \
    "$GLOBAL_SYMBOLS" \
    "local : * ; };")

echo "$F" >${2}
