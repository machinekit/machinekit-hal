#!/usr/bin/env python3
import os
import time
import pytest
from machinekit import rtapi
from machinekit import hal

from configparser import ConfigParser


uuid = None  # initialized by test_get_uuid
rt = None

@pytest.mark.usefixtures("realtime")
class Tests(object):
    def test_get_uuid(self):
        global uuid, rt
        cfg = ConfigParser()
        cfg.read(os.getenv("MACHINEKIT_INI"))
        uuid = cfg.get("MACHINEKIT", "MKUUID")


    def test_rtapi_connect(self):
        global rt
        rt = rtapi.RTAPIcommand(uuid=uuid)


    def test_loadrt_or2(self):
        global rt
        rt.newinst("or2", "or2.0")
        rt.newthread("servo-thread", 1000000, fp=True)
        hal.addf("or2.0", "servo-thread")
        hal.start_threads()
        time.sleep(0.2)


    def test_unloadrt_or2(self):
        hal.stop_threads()
        rt.delthread("servo-thread")
        rt.unloadrt("or2")
