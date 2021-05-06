# coding=utf-8
import os
from six.moves import configparser
from machinekit.hal import cyruntime, cyhal

# retrieve the machinekit UUID
cfg = configparser.ConfigParser()
cfg.read(os.getenv("MACHINEKIT_INI"))
uuid = cfg.get("MACHINEKIT", "MKUUID")

# open a command channel to rtapi_app
rt = cyruntime.RTAPIcommand(uuid=uuid)
