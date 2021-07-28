#!/usr/bin/env python3

# icomp is the demo/test comp for the HAL instantiation API
# it has the following parameters:
# module level:
#    answer - int, default 42
#    text   - string, default "abcdef"
#
# instance level:
#    repeat - int, default 1
#    prefix - string, default "bar"

import pytest
import os
from configparser import ConfigParser

from machinekit import rtapi,hal

@pytest.mark.usefixtures("realtime")
class TestIcomp():
    @pytest.fixture
    def setUp(self):
        global rt
        self.cfg = ConfigParser()
        self.cfg.read(os.getenv("MACHINEKIT_INI"))
        self.uuid = self.cfg.get("MACHINEKIT", "MKUUID")
        rt = rtapi.RTAPIcommand(uuid=self.uuid)

    def test_params(self, setUp):
        global rt

        # verify module params, compiled-in defaults
        rt.loadrt("icomp");
        answer_value = hal.Pin("icomp.answer_value")
        assert answer_value.get() == 42
        text_len = hal.Pin("icomp.text_len")
        assert text_len.get() == 6
        rt.unloadrt("icomp");

        # verify module params, explicitly passed params
        rt.loadrt("icomp","answer=1234567", 'text="foobarbaz"')
        answer_value = hal.Pin("icomp.answer_value")
        assert answer_value.get() == 1234567
        text_len = hal.Pin("icomp.text_len")
        assert text_len.get() == 9
        rt.unloadrt("icomp");

        # verify instance default parameters
        # as compiled in
        rt.loadrt("icomp");
        rt.newinst("icomp","inst");

        repeat_value = hal.Pin("inst.repeat_value")
        assert repeat_value.get() == 1
        prefix_len = hal.Pin("inst.prefix_len")
        assert prefix_len.get() == 3

        # verify explicit instance params
        rt.newinst("icomp","explicit", "repeat=314", 'prefix="foobarbaz"' );

        repeat_value = hal.Pin("explicit.repeat_value")
        assert repeat_value.get() == 314
        prefix_len = hal.Pin("explicit.prefix_len")
        assert prefix_len.get() == 9
