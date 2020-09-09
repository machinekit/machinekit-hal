#!/usr/bin/env python3

import pytest

from machinekit import compat

@pytest.mark.usefixtures('realtime')
class TestCompat(object):

    def test_get_rtapi_config(self):
        ld = compat.get_rtapi_config('LIBEXEC_DIR')
        print(repr(ld))
        assert ld.endswith("libexec")

(lambda s=__import__('signal'):
     s.signal(s.SIGTERM, s.SIG_IGN))()
