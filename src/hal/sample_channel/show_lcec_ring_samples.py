# purpose, get and inspect samples from sample channel component
# prerequisits:
#
# terminal 1
# halrun -I sample_channel.hal
#
# terminal 2
# python2 show_lcec_ring_sample.py
#
# change pin values in hal in terminal 1

import os, time
from machinekit import hal
from machinetalk.protobuf.sample_pb2 import Sample
from machinetalk.protobuf.types_pb2 import *

# provide the name to attach to
name = "lcec.0.1.ring"
sample = 0
pinname = ""
try:
    # attach to existing ring
    r = hal.Ring(name)
    while True:
        # inspect the ring:
        for i in r:
            sample += 1
            print("Sample :%i" % sample)
            b = i.tobytes()
            s = Sample()
            s.ParseFromString(b)
            if (s.HasField("v_bool") == True):
                v = s.v_bool
                t = "bit"
            if (s.HasField("v_double") == True):
                v = s.v_double
                t = "flt"
            if (s.HasField("v_uint32") == True):
                v = s.v_uint32
                t = "u32"
            if (s.HasField("v_int32") == True):
                v = s.v_int32
                t = "s32"
            if (s.HasField("v_uint64") == True):
                v = s.v_uint64
                t = "u64"
            if (s.HasField("v_int64") == True):
                v = s.v_int64
                t = "s64"
            # print out something meaningfull
            if (s.HasField("v_string") == True):
                pinname = s.v_string
            print("time: %s\nPin %s of type %s = %s" % (s.timestamp, pinname, t, v))
            # consume the sample
            r.shift()
        time.sleep(0.01)

except NameError as e:
    print(e)
