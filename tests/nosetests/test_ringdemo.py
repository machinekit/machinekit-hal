#!/usr/bin/env python3
import os,time,sys

import pytest
from machinekit import rtapi,hal

from configparser import ConfigParser

@pytest.mark.usefixtures("realtime")
class Tests(object):
    def test_rtapi_connect(self):
        global uuid, rt
        cfg = ConfigParser()
        cfg.read(os.getenv("MACHINEKIT_INI"))
        uuid = cfg.get("MACHINEKIT", "MKUUID")
        rt = rtapi.RTAPIcommand(uuid=uuid)

    def test_loadrt_ringmods(self):
        global r1
        r1 = hal.Ring("test", size=16384)
        rt.loadrt("ringread",  "ring=test")
        rt.loadrt("ringwrite", "ring=test")
        rt.newinst("charge_pump", "charge-pump")

    def test_net(self):
        hal.net("square-wave","charge-pump.out","ringwrite.write")

    def test_runthread(self):
        cpe = hal.Pin("charge-pump.enable")
        cpe.set(0)

        rt.newthread("fast",1000000, fp=True)
        rt.newthread("slow",100000000, fp=True)
        hal.addf("ringread","fast")
        hal.addf("ringwrite","slow")
        hal.addf("charge-pump","slow")
        hal.start_threads()
        cpe.set(1)    # enable charge_pump
        time.sleep(3) # let rt thread write a bit to ring
