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
        assert len(instances) == 1
        rt.newinst("icomp","bar")
        assert len(instances) == 2
        rt.delinst("foo")
        assert len(instances) == 1
        c = hal.Component("icomp")
        for i in instances:
            assert c.id == i.owner_id
            assert c.name == i.owner().name
        assert "foo" in instances
        assert "bar" in instances
        assert instances["foo"].size > 0
        assert instances["bar"].size > 0
        try:
            x = instances["nonexistent"]
            raise "should not happen"
        except NameError:
            pass
