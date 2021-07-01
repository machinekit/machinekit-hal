# ##############################################################################
# Description:  CMakeLists.txt
#
# This file, 'CMakeLists.txt', implements build system rules for Machinekit-HAL
# project
#
# Copyright (C) 2021    Jakub Fišer  <jakub DOT fiser AT eryaf DOT com>
#
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU Lesser General Public License as published by the Free
# Software Foundation; either version 2.1 of the License, or (at your option)
# any later version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
# details.
#
# You should have received a copy of the GNU Lesser General Public License along
# with this library; if not, write to the Free Software Foundation, Inc., 51
# Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
#
# ##############################################################################

option(BUILD_RUNTIME_MATH_LIBRARY "Built the Runtime Math library." TRUE)

if(BUILD_RUNTIME_MATH_LIBRARY)
  add_library(runtime_math SHARED)
  add_library(${MACHINEKIT_HAL_NAMESPACE}::runtime::runtime_math ALIAS
              runtime_math)

  set(SOURCE_FILES
      ${CMAKE_CURRENT_SOURCE_DIR}/src/e_acos.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/e_acosh.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/e_asin.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/e_atan2.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/e_atanh.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/e_cosh.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/e_exp.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/e_fmod.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/e_gamma.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/e_gamma_r.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/e_hypot.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/e_j0.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/e_j1.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/e_jn.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/e_lgamma.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/e_lgamma_r.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/e_log10.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/e_log.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/e_pow.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/e_remainder.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/e_rem_pio2.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/e_scalb.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/e_sinh.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/e_sqrt.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/k_cos.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/k_rem_pio2.c
      # ${CMAKE_CURRENT_SOURCE_DIR}/src/krtapi_math.c TODO: Decide what to do
      # with it
      ${CMAKE_CURRENT_SOURCE_DIR}/src/k_sin.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/k_standard.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/k_tan.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/librtapi_math.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/powidf.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/s_asinh.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/s_atan.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/s_cbrt.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/s_ceil.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/s_copysign.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/s_cos.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/s_erf.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/s_expm1.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/s_fabs.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/s_finite.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/s_floor.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/s_fminmax.c
      # ${CMAKE_CURRENT_SOURCE_DIR}/src/s_frexp.c TODO: Decide what to do with
      # it
      ${CMAKE_CURRENT_SOURCE_DIR}/src/s_ilogb.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/s_ldexp.c
      # ${CMAKE_CURRENT_SOURCE_DIR}/src/s_lib_version.c TODO: Decide what to do
      # with it
      ${CMAKE_CURRENT_SOURCE_DIR}/src/s_log1p.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/s_logb.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/s_matherr.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/s_modf.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/s_nextafter.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/s_rint.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/s_scalbn.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/s_significand.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/s_sin.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/s_tan.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/s_tanh.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/w_acos.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/w_acosh.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/w_asin.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/w_atan2.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/w_atanh.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/w_cabs.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/w_cosh.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/w_drem.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/w_exp.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/w_fmod.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/w_gamma.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/w_gamma_r.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/w_hypot.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/w_j0.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/w_j1.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/w_jn.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/w_lgamma.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/w_lgamma_r.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/w_log10.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/w_log.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/w_pow.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/w_remainder.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/w_scalb.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/w_sinh.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/w_sqrt.c)

  set(PUBLIC_HEADER_FILES
      ${CMAKE_CURRENT_SOURCE_DIR}/include/runtime/rtapi_byteorder.h
      ${CMAKE_CURRENT_SOURCE_DIR}/include/runtime/rtapi_math64.h
      ${CMAKE_CURRENT_SOURCE_DIR}/include/runtime/rtapi_math.h)

  set(PRIVATE_HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/mathP.h)

  target_sources(
    runtime_math
    PUBLIC ${PUBLIC_HEADER_FILES}
    PRIVATE ${SOURCE_FILES} ${PRIVATE_HEADER_FILES})
  target_include_directories(
    runtime_math PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

  target_link_libraries(runtime_math PRIVATE runtime_api)

  target_compile_definitions(runtime_math PRIVATE "_IEEE_LIBM" "_ISOC99_SOURCE"
                                                  "_DEFAULT_SOURCE")
  # "O2" "Werror"

  set_target_properties(
    runtime_math
    PROPERTIES SOVERSION 0
               VERSION ${CMAKE_PROJECT_VERSION}
               PUBLIC_HEADER "${PUBLIC_HEADER_FILES}"
               PRIVATE_HEADER "${PRIVATE_HEADER_FILES}"
               POSITION_INDEPENDENT_CODE TRUE)
  install(
    TARGETS runtime_math
    EXPORT machinekit_hal
    LIBRARY DESTINATION "${MACHINEKIT_HAL_LIBRARY_DIRECTORY}"
            COMPONENT MachinekitHAL_Library_Base
            NAMELINK_COMPONENT MachinekitHAL_Library_Development
    PUBLIC_HEADER DESTINATION "${MACHINEKIT_HAL_INTERFACE_DIRECTORY}"
                  COMPONENT MachinekitHAL_Library_Development)

endif()
