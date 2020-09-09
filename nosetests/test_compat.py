#!/usr/bin/env python3

import pytest

from machinekit import compat

@pytest.mark.usefixtures('realtime')
class TestCompat(object):

    def test_compat(self):
        # nonexistant kernel module
        assert compat.is_module_loaded("foobarbaz") == False

(lambda s=__import__('signal'):
     s.signal(s.SIGTERM, s.SIG_IGN))()
