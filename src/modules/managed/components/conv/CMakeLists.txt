# ~~~
# ####################################################################
# Description:  CMakeLists.txt
#
#               This file, 'CMakeLists.txt', implements build system
#               rules for Machinekit-HAL project
#
# Copyright (C) 2021    Jakub Fišer  <jakub DOT fiser AT eryaf DOT com>
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
#
# #####################################################################
# ~~~

option(BUILD_CONVS_MANAGED_MODULE "Built the Conv set of HAL managed modules."
       TRUE)

if(BUILD_CONVS_MANAGED_MODULE)
  include(MachinekitHALInstcomp)
  set(CONV_ICOMP_IN "float")
  set(CONV_ICOMP_OUT "s32")
  set(CONV_ICOMP_CC "")
  set(CONV_ICOMP_FP "fp")
  set(CONV_ICOMP_MIN "-2147483647-1")
  set(CONV_ICOMP_MAX "2147483647")

  configure_file(src/conv.icomp.in build/src/conv_float_s32.icomp @ONLY
                 NEWLINE_STYLE UNIX NO_SOURCE_PERMISSIONS)

  add_instantiatable_module(
    conv_float_s32 SOURCE
    ${CMAKE_CURRENT_BINARY_DIR}/build/src/conv_float_s32.icomp OUTPUT_DIRECTORY
    "${MACHINEKIT_HAL_MANAGED_MODULE_OUTPUT_DIRECTORY}")

  add_dependencies(conv_float_s32 instcomp_executable)

  set(CONV_ICOMP_IN "float")
  set(CONV_ICOMP_OUT "u32")
  set(CONV_ICOMP_CC "")
  set(CONV_ICOMP_FP "fp")
  set(CONV_ICOMP_MIN "0")
  set(CONV_ICOMP_MAX "2147483647")

  configure_file(src/conv.icomp.in build/src/conv_float_u32.icomp @ONLY
                 NEWLINE_STYLE UNIX NO_SOURCE_PERMISSIONS)

  add_instantiatable_module(
    conv_float_u32 SOURCE
    ${CMAKE_CURRENT_BINARY_DIR}/build/src/conv_float_u32.icomp OUTPUT_DIRECTORY
    "${MACHINEKIT_HAL_MANAGED_MODULE_OUTPUT_DIRECTORY}")

  add_dependencies(conv_float_u32 instcomp_executable)

  set(CONV_ICOMP_IN "bit")
  set(CONV_ICOMP_OUT "s32")
  set(CONV_ICOMP_CC "")
  set(CONV_ICOMP_FP "")
  set(CONV_ICOMP_MIN "0")
  set(CONV_ICOMP_MAX "2147483647")

  configure_file(src/conv.icomp.in build/src/conv_bit_s32.icomp @ONLY
                 NEWLINE_STYLE UNIX NO_SOURCE_PERMISSIONS)

  add_instantiatable_module(
    conv_bit_s32 SOURCE
    ${CMAKE_CURRENT_BINARY_DIR}/build/src/conv_bit_s32.icomp OUTPUT_DIRECTORY
    "${MACHINEKIT_HAL_MANAGED_MODULE_OUTPUT_DIRECTORY}")

  add_dependencies(conv_bit_s32 instcomp_executable)

  set(CONV_ICOMP_IN "bit")
  set(CONV_ICOMP_OUT "u32")
  set(CONV_ICOMP_CC "")
  set(CONV_ICOMP_FP "")
  set(CONV_ICOMP_MIN "0")
  set(CONV_ICOMP_MAX "2147483647")

  configure_file(src/conv.icomp.in build/src/conv_bit_u32.icomp @ONLY
                 NEWLINE_STYLE UNIX NO_SOURCE_PERMISSIONS)

  add_instantiatable_module(
    conv_bit_u32 SOURCE
    ${CMAKE_CURRENT_BINARY_DIR}/build/src/conv_bit_u32.icomp OUTPUT_DIRECTORY
    "${MACHINEKIT_HAL_MANAGED_MODULE_OUTPUT_DIRECTORY}")

  add_dependencies(conv_bit_u32 instcomp_executable)

  set(CONV_ICOMP_IN "s32")
  set(CONV_ICOMP_OUT "float")
  set(CONV_ICOMP_CC "")
  set(CONV_ICOMP_FP "fp")
  set(CONV_ICOMP_MIN "0")
  set(CONV_ICOMP_MAX "2147483647")

  configure_file(src/conv.icomp.in build/src/conv_s32_float.icomp @ONLY
                 NEWLINE_STYLE UNIX NO_SOURCE_PERMISSIONS)

  add_instantiatable_module(
    conv_s32_float SOURCE
    ${CMAKE_CURRENT_BINARY_DIR}/build/src/conv_s32_float.icomp OUTPUT_DIRECTORY
    "${MACHINEKIT_HAL_MANAGED_MODULE_OUTPUT_DIRECTORY}")

  add_dependencies(conv_s32_float instcomp_executable)

  set(CONV_ICOMP_IN "s32")
  set(CONV_ICOMP_OUT "bit")
  set(CONV_ICOMP_CC "")
  set(CONV_ICOMP_FP "")
  set(CONV_ICOMP_MIN "0")
  set(CONV_ICOMP_MAX "1")

  configure_file(src/conv.icomp.in build/src/conv_s32_bit.icomp @ONLY
                 NEWLINE_STYLE UNIX NO_SOURCE_PERMISSIONS)

  add_instantiatable_module(
    conv_s32_bit SOURCE
    ${CMAKE_CURRENT_BINARY_DIR}/build/src/conv_s32_bit.icomp OUTPUT_DIRECTORY
    "${MACHINEKIT_HAL_MANAGED_MODULE_OUTPUT_DIRECTORY}")

  add_dependencies(conv_s32_bit instcomp_executable)

  set(CONV_ICOMP_IN "s32")
  set(CONV_ICOMP_OUT "u32")
  set(CONV_ICOMP_CC "")
  set(CONV_ICOMP_FP "")
  set(CONV_ICOMP_MIN "0")
  set(CONV_ICOMP_MAX "0")

  configure_file(src/conv.icomp.in build/src/conv_s32_u32.icomp @ONLY
                 NEWLINE_STYLE UNIX NO_SOURCE_PERMISSIONS)

  add_instantiatable_module(
    conv_s32_u32 SOURCE
    ${CMAKE_CURRENT_BINARY_DIR}/build/src/conv_s32_u32.icomp OUTPUT_DIRECTORY
    "${MACHINEKIT_HAL_MANAGED_MODULE_OUTPUT_DIRECTORY}")

  add_dependencies(conv_s32_u32 instcomp_executable)

  set(CONV_ICOMP_IN "u32")
  set(CONV_ICOMP_OUT "float")
  set(CONV_ICOMP_CC "")
  set(CONV_ICOMP_FP "fp")
  set(CONV_ICOMP_MIN "0")
  set(CONV_ICOMP_MAX "0")

  configure_file(src/conv.icomp.in build/src/conv_u32_float.icomp @ONLY
                 NEWLINE_STYLE UNIX NO_SOURCE_PERMISSIONS)

  add_instantiatable_module(
    conv_u32_float SOURCE
    ${CMAKE_CURRENT_BINARY_DIR}/build/src/conv_u32_float.icomp OUTPUT_DIRECTORY
    "${MACHINEKIT_HAL_MANAGED_MODULE_OUTPUT_DIRECTORY}")

  add_dependencies(conv_u32_float instcomp_executable)

  set(CONV_ICOMP_IN "u32")
  set(CONV_ICOMP_OUT "bit")
  set(CONV_ICOMP_CC "")
  set(CONV_ICOMP_FP "")
  set(CONV_ICOMP_MIN "-1")
  set(CONV_ICOMP_MAX "1")

  configure_file(src/conv.icomp.in build/src/conv_u32_bit.icomp @ONLY
                 NEWLINE_STYLE UNIX NO_SOURCE_PERMISSIONS)

  add_instantiatable_module(
    conv_u32_bit SOURCE
    ${CMAKE_CURRENT_BINARY_DIR}/build/src/conv_u32_bit.icomp OUTPUT_DIRECTORY
    "${MACHINEKIT_HAL_MANAGED_MODULE_OUTPUT_DIRECTORY}")

  add_dependencies(conv_u32_bit instcomp_executable)

  set(CONV_ICOMP_IN "u32")
  set(CONV_ICOMP_OUT "s32")
  set(CONV_ICOMP_CC "")
  set(CONV_ICOMP_FP "")
  set(CONV_ICOMP_MIN "-1")
  set(CONV_ICOMP_MAX "2147483647")

  configure_file(src/conv.icomp.in build/src/conv_u32_s32.icomp @ONLY
                 NEWLINE_STYLE UNIX NO_SOURCE_PERMISSIONS)

  add_instantiatable_module(
    conv_u32_s32 SOURCE
    ${CMAKE_CURRENT_BINARY_DIR}/build/src/conv_u32_s32.icomp OUTPUT_DIRECTORY
    "${MACHINEKIT_HAL_MANAGED_MODULE_OUTPUT_DIRECTORY}")

  add_dependencies(conv_u32_s32 instcomp_executable)

  set(CONV_ICOMP_IN "float")
  set(CONV_ICOMP_OUT "s64")
  set(CONV_ICOMP_CC "")
  set(CONV_ICOMP_FP "fp")
  set(CONV_ICOMP_MIN "-9223372036854775807-1")
  set(CONV_ICOMP_MAX "9223372036854775807")

  configure_file(src/conv.icomp.in build/src/conv_float_s64.icomp @ONLY
                 NEWLINE_STYLE UNIX NO_SOURCE_PERMISSIONS)

  add_instantiatable_module(
    conv_float_s64 SOURCE
    ${CMAKE_CURRENT_BINARY_DIR}/build/src/conv_float_s64.icomp OUTPUT_DIRECTORY
    "${MACHINEKIT_HAL_MANAGED_MODULE_OUTPUT_DIRECTORY}")

  add_dependencies(conv_float_s64 instcomp_executable)

  set(CONV_ICOMP_IN "float")
  set(CONV_ICOMP_OUT "u64")
  set(CONV_ICOMP_CC "")
  set(CONV_ICOMP_FP "fp")
  set(CONV_ICOMP_MIN "0")
  set(CONV_ICOMP_MAX "18446744073709551615UL")

  configure_file(src/conv.icomp.in build/src/conv_float_u64.icomp @ONLY
                 NEWLINE_STYLE UNIX NO_SOURCE_PERMISSIONS)

  add_instantiatable_module(
    conv_float_u64 SOURCE
    ${CMAKE_CURRENT_BINARY_DIR}/build/src/conv_float_u64.icomp OUTPUT_DIRECTORY
    "${MACHINEKIT_HAL_MANAGED_MODULE_OUTPUT_DIRECTORY}")

  add_dependencies(conv_float_u64 instcomp_executable)

  set(CONV_ICOMP_IN "bit")
  set(CONV_ICOMP_OUT "s64")
  set(CONV_ICOMP_CC "")
  set(CONV_ICOMP_FP "")
  set(CONV_ICOMP_MIN "0")
  set(CONV_ICOMP_MAX "18446744073709551615UL")

  configure_file(src/conv.icomp.in build/src/conv_bit_s64.icomp @ONLY
                 NEWLINE_STYLE UNIX NO_SOURCE_PERMISSIONS)

  add_instantiatable_module(
    conv_bit_s64 SOURCE
    ${CMAKE_CURRENT_BINARY_DIR}/build/src/conv_bit_s64.icomp OUTPUT_DIRECTORY
    "${MACHINEKIT_HAL_MANAGED_MODULE_OUTPUT_DIRECTORY}")

  add_dependencies(conv_bit_s64 instcomp_executable)

  set(CONV_ICOMP_IN "bit")
  set(CONV_ICOMP_OUT "u64")
  set(CONV_ICOMP_CC "")
  set(CONV_ICOMP_FP "")
  set(CONV_ICOMP_MIN "0")
  set(CONV_ICOMP_MAX "18446744073709551615UL")

  configure_file(src/conv.icomp.in build/src/conv_bit_u64.icomp @ONLY
                 NEWLINE_STYLE UNIX NO_SOURCE_PERMISSIONS)

  add_instantiatable_module(
    conv_bit_u64 SOURCE
    ${CMAKE_CURRENT_BINARY_DIR}/build/src/conv_bit_u64.icomp OUTPUT_DIRECTORY
    "${MACHINEKIT_HAL_MANAGED_MODULE_OUTPUT_DIRECTORY}")

  add_dependencies(conv_bit_u64 instcomp_executable)

  set(CONV_ICOMP_IN "s64")
  set(CONV_ICOMP_OUT "float")
  set(CONV_ICOMP_CC "")
  set(CONV_ICOMP_FP "fp")
  set(CONV_ICOMP_MIN "0")
  set(CONV_ICOMP_MAX "18446744073709551615UL")

  configure_file(src/conv.icomp.in build/src/conv_s64_float.icomp @ONLY
                 NEWLINE_STYLE UNIX NO_SOURCE_PERMISSIONS)

  add_instantiatable_module(
    conv_s64_float SOURCE
    ${CMAKE_CURRENT_BINARY_DIR}/build/src/conv_s64_float.icomp OUTPUT_DIRECTORY
    "${MACHINEKIT_HAL_MANAGED_MODULE_OUTPUT_DIRECTORY}")

  add_dependencies(conv_s64_float instcomp_executable)

  set(CONV_ICOMP_IN "s64")
  set(CONV_ICOMP_OUT "bit")
  set(CONV_ICOMP_CC "")
  set(CONV_ICOMP_FP "")
  set(CONV_ICOMP_MIN "0")
  set(CONV_ICOMP_MAX "1")

  configure_file(src/conv.icomp.in build/src/conv_s64_bit.icomp @ONLY
                 NEWLINE_STYLE UNIX NO_SOURCE_PERMISSIONS)

  add_instantiatable_module(
    conv_s64_bit SOURCE
    ${CMAKE_CURRENT_BINARY_DIR}/build/src/conv_s64_bit.icomp OUTPUT_DIRECTORY
    "${MACHINEKIT_HAL_MANAGED_MODULE_OUTPUT_DIRECTORY}")

  add_dependencies(conv_s64_bit instcomp_executable)

  set(CONV_ICOMP_IN "u64")
  set(CONV_ICOMP_OUT "float")
  set(CONV_ICOMP_CC "")
  set(CONV_ICOMP_FP "fp")
  set(CONV_ICOMP_MIN "0")
  set(CONV_ICOMP_MAX "1")

  configure_file(src/conv.icomp.in build/src/conv_u64_float.icomp @ONLY
                 NEWLINE_STYLE UNIX NO_SOURCE_PERMISSIONS)

  add_instantiatable_module(
    conv_u64_float SOURCE
    ${CMAKE_CURRENT_BINARY_DIR}/build/src/conv_u64_float.icomp OUTPUT_DIRECTORY
    "${MACHINEKIT_HAL_MANAGED_MODULE_OUTPUT_DIRECTORY}")

  add_dependencies(conv_u64_float instcomp_executable)

  set(CONV_ICOMP_IN "u64")
  set(CONV_ICOMP_OUT "bit")
  set(CONV_ICOMP_CC "")
  set(CONV_ICOMP_FP "")
  set(CONV_ICOMP_MIN "-1")
  set(CONV_ICOMP_MAX "1")

  configure_file(src/conv.icomp.in build/src/conv_u64_bit.icomp @ONLY
                 NEWLINE_STYLE UNIX NO_SOURCE_PERMISSIONS)

  add_instantiatable_module(
    conv_u64_bit SOURCE
    ${CMAKE_CURRENT_BINARY_DIR}/build/src/conv_u64_bit.icomp OUTPUT_DIRECTORY
    "${MACHINEKIT_HAL_MANAGED_MODULE_OUTPUT_DIRECTORY}")

  add_dependencies(conv_u64_bit instcomp_executable)

  set(CONV_ICOMP_IN "u64")
  set(CONV_ICOMP_OUT "s64")
  set(CONV_ICOMP_CC "")
  set(CONV_ICOMP_FP "")
  set(CONV_ICOMP_MIN "-1")
  set(CONV_ICOMP_MAX "9223372036854775807")

  configure_file(src/conv.icomp.in build/src/conv_u64_s64.icomp @ONLY
                 NEWLINE_STYLE UNIX NO_SOURCE_PERMISSIONS)

  add_instantiatable_module(
    conv_u64_s64 SOURCE
    ${CMAKE_CURRENT_BINARY_DIR}/build/src/conv_u64_s64.icomp OUTPUT_DIRECTORY
    "${MACHINEKIT_HAL_MANAGED_MODULE_OUTPUT_DIRECTORY}")

  add_dependencies(conv_u64_s64 instcomp_executable)

  set(CONV_ICOMP_IN "s64")
  set(CONV_ICOMP_OUT "u64")
  set(CONV_ICOMP_CC "")
  set(CONV_ICOMP_FP "")
  set(CONV_ICOMP_MIN "0")
  set(CONV_ICOMP_MAX "0")

  configure_file(src/conv.icomp.in build/src/conv_s64_u64.icomp @ONLY
                 NEWLINE_STYLE UNIX NO_SOURCE_PERMISSIONS)

  add_instantiatable_module(
    conv_s64_u64 SOURCE
    ${CMAKE_CURRENT_BINARY_DIR}/build/src/conv_s64_u64.icomp OUTPUT_DIRECTORY
    "${MACHINEKIT_HAL_MANAGED_MODULE_OUTPUT_DIRECTORY}")

  add_dependencies(conv_s64_u64 instcomp_executable)

  set(CONV_ICOMP_IN "u64")
  set(CONV_ICOMP_OUT "s32")
  set(CONV_ICOMP_CC "")
  set(CONV_ICOMP_FP "")
  set(CONV_ICOMP_MIN "0")
  set(CONV_ICOMP_MAX "2147483647")

  configure_file(src/conv.icomp.in build/src/conv_u64_s32.icomp @ONLY
                 NEWLINE_STYLE UNIX NO_SOURCE_PERMISSIONS)

  add_instantiatable_module(
    conv_u64_s32 SOURCE
    ${CMAKE_CURRENT_BINARY_DIR}/build/src/conv_u64_s32.icomp OUTPUT_DIRECTORY
    "${MACHINEKIT_HAL_MANAGED_MODULE_OUTPUT_DIRECTORY}")

  add_dependencies(conv_u64_s32 instcomp_executable)

  set(CONV_ICOMP_IN "s64")
  set(CONV_ICOMP_OUT "s32")
  set(CONV_ICOMP_CC "")
  set(CONV_ICOMP_FP "")
  set(CONV_ICOMP_MIN "-2147483647-1")
  set(CONV_ICOMP_MAX "2147483647")

  configure_file(src/conv.icomp.in build/src/conv_s64_s32.icomp @ONLY
                 NEWLINE_STYLE UNIX NO_SOURCE_PERMISSIONS)

  add_instantiatable_module(
    conv_s64_s32 SOURCE
    ${CMAKE_CURRENT_BINARY_DIR}/build/src/conv_s64_s32.icomp OUTPUT_DIRECTORY
    "${MACHINEKIT_HAL_MANAGED_MODULE_OUTPUT_DIRECTORY}")

  add_dependencies(conv_s64_s32 instcomp_executable)

  set(CONV_ICOMP_IN "u64")
  set(CONV_ICOMP_OUT "u32")
  set(CONV_ICOMP_CC "")
  set(CONV_ICOMP_FP "")
  set(CONV_ICOMP_MIN "0")
  set(CONV_ICOMP_MAX "4294967295UL")

  configure_file(src/conv.icomp.in build/src/conv_u64_u32.icomp @ONLY
                 NEWLINE_STYLE UNIX NO_SOURCE_PERMISSIONS)

  add_instantiatable_module(
    conv_u64_u32 SOURCE
    ${CMAKE_CURRENT_BINARY_DIR}/build/src/conv_u64_u32.icomp OUTPUT_DIRECTORY
    "${MACHINEKIT_HAL_MANAGED_MODULE_OUTPUT_DIRECTORY}")

  add_dependencies(conv_u64_u32 instcomp_executable)

  set(CONV_ICOMP_IN "s64")
  set(CONV_ICOMP_OUT "u32")
  set(CONV_ICOMP_CC "")
  set(CONV_ICOMP_FP "")
  set(CONV_ICOMP_MIN "0")
  set(CONV_ICOMP_MAX "4294967295UL")

  configure_file(src/conv.icomp.in build/src/conv_s64_u32.icomp @ONLY
                 NEWLINE_STYLE UNIX NO_SOURCE_PERMISSIONS)

  add_instantiatable_module(
    conv_s64_u32 SOURCE
    ${CMAKE_CURRENT_BINARY_DIR}/build/src/conv_s64_u32.icomp OUTPUT_DIRECTORY
    "${MACHINEKIT_HAL_MANAGED_MODULE_OUTPUT_DIRECTORY}")

  add_dependencies(conv_s64_u32 instcomp_executable)
endif()
