#!/usr/bin/env python3

# verify the cython inst bindings

import pytest
import time,os
from configparser import ConfigParser

from machinekit import rtapi,hal

@pytest.mark.usefixtures("realtime")
class TestIinst():
    def test_inst(self):
        self.cfg = ConfigParser()
        self.cfg.read(os.getenv("MACHINEKIT_INI"))
        self.uuid = self.cfg.get("MACHINEKIT", "MKUUID")
        rt = rtapi.RTAPIcommand(uuid=self.uuid)

        rt.loadrt("icomp");
        rt.newinst("icomp","foo")
        assert len(hal.instances) == 1
        rt.newinst("icomp","bar")
        assert len(hal.instances) == 2
        rt.delinst("foo")
        assert len(hal.instances) == 1
        with pytest.raises(RuntimeError):
            # HAL error: duplicate component name 'icomp'
            c = hal.Component("icomp")
        c = hal.components["icomp"]
        for name in hal.instances:
            inst = hal.instances[name]
            assert c.id == inst.owner_id
            assert c.name == inst.owner.name
        assert "foo" in hal.instances
        assert "bar" in hal.instances
        assert hal.instances["foo"].size > 0
        assert hal.instances["bar"].size > 0
        try:
            x = hal.instances["nonexistent"]
            raise "should not happen"
        except NameError:
            pass
