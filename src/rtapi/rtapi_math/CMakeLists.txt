set(RTAPI_MATH_SRC
    e_acos.c
    e_asin.c
    e_atan2.c
    e_fmod.c
    e_hypot.c
    e_pow.c
    e_rem_pio2.c
    e_sqrt.c
    k_cos.c
    k_rem_pio2.c
    k_sin.c
    k_standard.c
    k_tan.c
    librtapi_math.c
    powidf.c
    s_atan.c
    s_cbrt.c
    s_ceil.c
    s_copysign.c
    s_cos.c
    s_fabs.c
    s_fabs.c
    s_finite.c
    s_floor.c
    s_fminmax.c
    s_rint.c
    s_rint.c
    s_scalbn.c
    s_sin.c
    s_tan.c
    w_acos.c
    w_asin.c
    w_atan2.c
    w_fmod.c
    w_hypot.c
    w_pow.c
    w_sqrt.c)

add_library(rtapi_math SHARED ${RTAPI_MATH_SRC})
set_target_properties(rtapi_math PROPERTIES COMPILE_FLAGS "-Werror")
target_compile_definitions(rtapi_math PRIVATE "_IEEE_LIBM;_ISOC99_SOURCE;_DEFAULT_SOURCE")
set_target_properties(rtapi_math PROPERTIES VERSION ${GENERIC_LIB_VERSION})
_install(rtapi_math)
