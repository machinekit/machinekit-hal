#!/bin/bash -e
#
# Run unit tests
#
# Tests need to run file-by-file as separate Python processes because once HAL
# and RTAPI are loaded into Python, it's impossible to clear out state, even
# restarting `realtime`, since `libhal` etc. are now loaded.

PYTEST=pytest-3
TESTDIR="$(dirname $0)"

TESTS=""
for ARG in "${@}"; do
    echo $ARG
    if [[ "$ARG" =~ ^test_.*\.py$ ]]; then
        echo Adding test $ARG
        TESTS+=" $ARG"
        shift
    elif [ "$ARG" = "--" ]; then
        shift
        break
    else
        echo "Usage:  $0 [test_foo.py ...] [-- --pytest-arg ...]" >&2
        exit 1
    fi
done

if test -z "$TESTS"; then
    TESTS="
        test_compat.py
        test_mk_hal_basics.py
        test_pinops.py
        test_netcmd.py
        test_groups.py
        test_rtapi.py
        test_icomp.py
        test_instbindings.py
        test_or2.py
        test_ring.py
        test_ring_intracomp.py
        test_ringdemo.py
        test_streamring.py
    "
fi

# Debug output
export SYSLOG_TO_STDERR=1

for TEST in $TESTS; do
    TESTPATH="${TESTDIR}"/$TEST
    echo +++++++++ Running $PYTEST $TESTPATH "${@}"
    $PYTEST $TESTPATH "${@}"
done
