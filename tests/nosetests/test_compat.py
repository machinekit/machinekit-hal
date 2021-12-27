import pytest
import pathlib

import machinekit.hal.cycompat as compat

@pytest.mark.usefixtures('realtime')
class TestCompat(object):

    def test_get_rtapi_config(self):
        ld = compat.get_rtapi_config('LIBEXEC_DIR')
        print(repr(ld))
        assert pathlib.Path(ld).exists()

(lambda s=__import__('signal'):
     s.signal(s.SIGTERM, s.SIG_IGN))()
