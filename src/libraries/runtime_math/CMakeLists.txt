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
      src/e_acos.c
      src/e_acosh.c
      src/e_asin.c
      src/e_atan2.c
      src/e_atanh.c
      src/e_cosh.c
      src/e_exp.c
      src/e_fmod.c
      src/e_gamma.c
      src/e_gamma_r.c
      src/e_hypot.c
      src/e_j0.c
      src/e_j1.c
      src/e_jn.c
      src/e_lgamma.c
      src/e_lgamma_r.c
      src/e_log10.c
      src/e_log.c
      src/e_pow.c
      src/e_remainder.c
      src/e_rem_pio2.c
      src/e_scalb.c
      src/e_sinh.c
      src/e_sqrt.c
      src/k_cos.c
      src/k_rem_pio2.c
      # src/krtapi_math.c TODO: Decide what to do with it
      src/k_sin.c
      src/k_standard.c
      src/k_tan.c
      src/librtapi_math.c
      src/powidf.c
      src/s_asinh.c
      src/s_atan.c
      src/s_cbrt.c
      src/s_ceil.c
      src/s_copysign.c
      src/s_cos.c
      src/s_erf.c
      src/s_expm1.c
      src/s_fabs.c
      src/s_finite.c
      src/s_floor.c
      src/s_fminmax.c
      # src/s_frexp.c TODO: Decide what to do with it
      src/s_ilogb.c
      src/s_ldexp.c
      # src/s_lib_version.c TODO: Decide what to do with it
      src/s_log1p.c
      src/s_logb.c
      src/s_matherr.c
      src/s_modf.c
      src/s_nextafter.c
      src/s_rint.c
      src/s_scalbn.c
      src/s_significand.c
      src/s_sin.c
      src/s_tan.c
      src/s_tanh.c
      src/w_acos.c
      src/w_acosh.c
      src/w_asin.c
      src/w_atan2.c
      src/w_atanh.c
      src/w_cabs.c
      src/w_cosh.c
      src/w_drem.c
      src/w_exp.c
      src/w_fmod.c
      src/w_gamma.c
      src/w_gamma_r.c
      src/w_hypot.c
      src/w_j0.c
      src/w_j1.c
      src/w_jn.c
      src/w_lgamma.c
      src/w_lgamma_r.c
      src/w_log10.c
      src/w_log.c
      src/w_pow.c
      src/w_remainder.c
      src/w_scalb.c
      src/w_sinh.c
      src/w_sqrt.c)

  target_sources(runtime_math PRIVATE ${SOURCE_FILES})
  target_include_directories(
    runtime_math PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

  target_link_libraries(runtime_math PRIVATE runtime_api)

  target_compile_definitions(runtime_math PRIVATE "_IEEE_LIBM" "_ISOC99_SOURCE"
                                                  "_DEFAULT_SOURCE")
  # "O2" "Werror"

  set_target_properties(
    runtime_math
    PROPERTIES SOVERSION 0
               PUBLIC_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/include/rtapi_math.h"
               PUBLIC_HEADER
               "${CMAKE_CURRENT_SOURCE_DIR}/include/rtapi_math64.h"
               POSITION_INDEPENDENT_CODE TRUE)
endif()
