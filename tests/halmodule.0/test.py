#!/usr/bin/env python3
import hal
import os

h = hal.component("x")
try:
    pins = dict(
        HAL_S32 = h.newpin("HAL_S32", hal.HAL_S32, hal.HAL_OUT),
        HAL_U32 = h.newpin("HAL_U32", hal.HAL_U32, hal.HAL_OUT),
        HAL_S64 = h.newpin("HAL_S64", hal.HAL_S64, hal.HAL_OUT),
        HAL_U64 = h.newpin("HAL_U64", hal.HAL_U64, hal.HAL_OUT),
        HAL_FLOAT = h.newpin("HAL_FLOAT", hal.HAL_FLOAT, hal.HAL_OUT)
    )
    param = h.newparam("param", hal.HAL_BIT, hal.HAL_RW)
    h.ready()

    ########## Set and read pin values; access pins via hal.compenent.__getitem__()
    def try_set(p, v):
        try:
            h[p] = v
            hp = h[p]
            print("set {} {} {}".format(p, v, "ok" if hp == v else repr(hp)))
        except ValueError as e:
            print("set {} {} {}".format(p, v, "ValueError: %s" % e))
        except OverflowError as e:
            print("set {} {} {}".format(p, v, "OverflowError: %s" % e))

    print("S32 pass")
    # pass
    try_set("HAL_S32", -1)
    try_set("HAL_S32", 0)
    try_set("HAL_S32", 1)
    try_set("HAL_S32", 0x7fffffff) # INT32_MAX
    try_set("HAL_S32", -0x80000000) # INT32_MIN
    print("S32 fail")
    try_set("HAL_S32", 0x80000000) # INT32_MAX + 1
    try_set("HAL_S32", -0x80000001) # INT32_MIN - 1

    print("U32 pass")
    try_set("HAL_U32", 0)
    try_set("HAL_U32", 1)
    try_set("HAL_U32", 0xffffffff) # UINT32_MAX
    print("U32 fail")
    try_set("HAL_U32", -1) # Negative
    try_set("HAL_U32", 1 <<32) # UINT32_MAX + 1

    print("S64 pass")
    # pass
    try_set("HAL_S64", -1)
    try_set("HAL_S64", 0)
    try_set("HAL_S64", 1)
    try_set("HAL_S64", 0x7fffffffffffffff) # INT64_MAX
    try_set("HAL_S64", -0x8000000000000000) # INT64_MIN
    print("S64 fail")
    try_set("HAL_S64", 0x8000000000000000) # INT64_MAX + 1
    try_set("HAL_S64", -0x8000000000000001) # INT64_MIN - 1

    print("U64 pass")
    try_set("HAL_U64", 0)
    try_set("HAL_U64", 1)
    try_set("HAL_U64", 0xffffffffffffffff) # UINT64_MAX
    print("U64 fail")
    try_set("HAL_U64", -1) # Negative
    try_set("HAL_U64", 1 <<64) # UINT64_MAX + 1

    print("FLOAT pass")
    try_set("HAL_FLOAT", 0)
    try_set("HAL_FLOAT", 0.0)
    try_set("HAL_FLOAT", -1)
    try_set("HAL_FLOAT", -1.0)
    try_set("HAL_FLOAT", 1)
    try_set("HAL_FLOAT", 1.0)
    try_set("HAL_FLOAT", 1 <<1023) # Largest float (?)
    print("FLOAT fail")
    try_set("HAL_FLOAT", 1 <<1024) # 2x largest float (?)

    ########## Check pin and param attributes and hal.component.getitem() accessor
    print("Validate pins")
    def pin_validate(i, t, d):
        print("pincheck {} {} {} {}".format(
            i.get_name(), i.is_pin(), i.get_type() == t, i.get_dir() == d))

    for pin_name in pins:
        pin_type = getattr(hal, pin_name)

        # Check pin object directly
        pin_validate(pins[pin_name], pin_type, hal.HAL_OUT)

        # Check getitem()
        pin = h.getitem(pin_name)
        pin_validate(pin, pin_type, hal.HAL_OUT)

    # Test getitem() failure
    try:
        pin = h.getitem("not-found")
        print("{} {} {}".format("getitem", "not-found", "ok"))
    except:
        print("{} {} {}".format("getitem", "not-found", "fail"))

    # Test params and getitem(param)
    pin_validate(param, hal.HAL_BIT, hal.HAL_RW)
    param = h.getitem("param")
    pin_validate(param, hal.HAL_BIT, hal.HAL_RW)

    ########## Check pin set(), get(), get_name() methods
    print("Validate set/get")
    def try_set_pin(p, v):
        try:
            p.set(v)
            print("setpin {} {} {}".format(p.get_name(), v, p.get()))
        except ValueError as e:
            print("setpin {} {} {}".format(p.get_name(), v, "ValueError: %s" % e))
        except OverflowError as e:
            print("setpin {} {} {}".format(p.get_name(), v, "OverflowError: %s" % e))

    try_set_pin(pins["HAL_U32"], 0)
    try_set_pin(pins["HAL_U32"], -1)
except:
    import traceback
    print("Exception: {}".format(traceback.format_exc()))
    raise
finally:
    h.exit()
