# rtapi_compat.c  python bindings

from os import strerror


def get_rtapi_config(param):
    cdef char result[1024]
    rc = c_get_rtapi_config(result, param, 1024)
    if rc:
        raise RuntimeError(f"get_rtapiconfig({param}) failed: {rc} {strerror(-rc)}")
    return str(<char *>result)
