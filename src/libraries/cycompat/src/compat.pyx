# rtapi_compat.c  python bindings

from os import strerror


def get_rtapi_config(param):
    cdef char result[1024]
    param_b = param.encode()
    rc = c_get_rtapi_config(result, param_b, 1024)
    if rc:
        raise RuntimeError(f"get_rtapiconfig({param}) failed: {rc} {strerror(-rc)}")
    return result.decode()
