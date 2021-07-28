# how to iterate over a ring:
#
# peeking at the contents
# consuming all contents
#
# run as:
# halrun -I ringwrite.hal
# python ringread.py

import os, time
import machinekit.hal.cyhal as hal

def main():
    name = "ring_0"
    try:
        # attach to existing ring
        r = hal.Ring(name)
    except NameError as e:
        print(e)
    
    else:
        while True:
            # peek at the ring contents:
            for i in r:
                print("peek record: ",i.tobytes())
    
            # then consume all records available
            for i in r:
                print("consume record: ",i.tobytes())
                r.shift()
    
            time.sleep(1)

if __name__ == "__main__":
    main()
