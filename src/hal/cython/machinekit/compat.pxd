# vim: sts=4 sw=4 et
# cython: language_level=3

cdef extern from "rtapi_bitops.h":
    int RTAPI_BIT(int b)

cdef extern from "rtapi_compat.h":
    int c_get_rtapi_config "get_rtapi_config" (char *result, const char *param, int n)
