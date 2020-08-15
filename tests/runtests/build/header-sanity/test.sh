#!/bin/sh
set -xe

HEADERS_DIRECTORY=$(readlink -f ../../../include)
mapfile -d '' -t ARRAY_H_HH < <(find ${HEADERS_DIRECTORY} -type f \
    \( -iname \*.h -o -iname \*.hh \) -print0 | \
    xargs -0 -n1 -I '{}' realpath -e -z '{}')

if (( ${#ARRAY_H_HH[@]} <= 5 )); then
    printf "Found %d header files:\n===\n" ${#ARRAY_H_HH[@]}
    for FILE in "${ARRAY_H_HH[@]}"
    do
        printf "FILE: %s\n" "$FILE"
    done
    printf "This seems too low number\n"
    exit 2
fi

for HEADER in "${ARRAY_H_HH[@]}"
do
    case ${HEADER} in
    */rtapi_app.h) continue ;;
    */rtapi_common.h) continue ;;
    */*.pb.h) continue ;;
    */container.h) continue ;;
    esac
    if [ "${HEADER: -2}" == ".h" ]
    then
        gcc -DULAPI -I${HEADERS_DIRECTORY} -E -x c ${HEADER} > /dev/null
    fi
    g++ -DULAPI -I${HEADERS_DIRECTORY} -E -x c++ ${HEADER} > /dev/null
done
