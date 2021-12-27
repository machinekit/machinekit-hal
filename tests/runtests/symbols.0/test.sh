#!/bin/sh
set -xe
! MSGD_OPTS="--stderr" halrun dotest.hal
