#!/usr/bin/env python3

# create a ring
# assure records written can be read back

import pytest
from machinekit import hal

@pytest.mark.usefixtures('realtime')
class TestCompat(object):

    def test_create_ring(self):
        global r1
        # size given mean - create existing ring
        r1 = hal.Ring("ring1", size=4096)
        # leave around - reused below

    def test_ring_write_read(self):
        nr = 0
        count = 100
        for n in range(count):
            r1.write("record %d" % n)
            record = r1.read()
            if record is None:
                raise RuntimeError("no record after write %d" % n)
            nr += 1
            r1.shift()
        assert nr == count
        record = r1.read()
        assert record is None # ring must be empty
