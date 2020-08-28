
import sys

from machinekit import hal

name = "ring1"
ringsize = 16384
polltime = 0.1

try:
    # to attach if no size given
    r = hal.Ring(name)
except RuntimeError as e:
    # else create
    r = hal.Ring(name, size=ringsize)

count = 10
for n in range(count):
    try:
        r.write("record %d" % n)
    except RuntimeError as e:
        print(e)
