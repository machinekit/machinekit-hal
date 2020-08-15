#!/usr/bin/env python3

# create a ring
# assure records written can be read back

import pytest
from machinekit import hal

size=4096

@pytest.mark.usefixtures("realtime")
class Tests(object):
    def test_create_ring(self):
        global r,sr
        r = hal.Ring("ring1", size=size, type=hal.RINGTYPE_STREAM)
        sr = hal.StreamRing(r)

    def test_ring_write_read(self):
        nr = 0
        for n in range(size):
            if sr.write("X") < 1:
                assert n == size - 1

        m = sr.read()
        assert len(m) == size -1
