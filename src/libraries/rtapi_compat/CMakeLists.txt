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

# Temporary INTERFACE library to solve Machinekit-HAL issue with symbol
# placement

# Transformed into INTERFACE library to manage dependencies

add_library(rtapi_compat INTERFACE)
add_library(${MACHINEKIT_HAL_NAMESPACE}::rtapi_compat ALIAS rtapi_compat)

set(SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/rtapi_compat.c")

set(PUBLIC_INCLUDE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include")

set(PUBLIC_HEADER_FILES_BUILD
    "${PUBLIC_INCLUDE_DIRECTORY}/runtime/rtapi_compat.h")

set(PUBLIC_HEADER_FILES_INSTALL
    "${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/runtime/rtapi_compat.h")

target_sources(
  rtapi_compat
  INTERFACE $<BUILD_INTERFACE:${PUBLIC_HEADER_FILES_BUILD}>
            $<INSTALL_INTERFACE:${PUBLIC_HEADER_FILES_INSTALL}>
            $<BUILD_INTERFACE:${SOURCE_FILES}>)

target_include_directories(
  rtapi_compat
  INTERFACE $<BUILD_INTERFACE:${PUBLIC_INCLUDE_DIRECTORY}>
            $<INSTALL_INTERFACE:${MACHINEKIT_HAL_INTERFACE_DIRECTORY}>)

target_link_libraries(
  rtapi_compat INTERFACE mkini runtime_config) # TODO: This will pollute the
                                               # managed namespace with mkini
                                               # symbols, is this what is
                                               # wanted?

set_target_properties(
  rtapi_compat
  PROPERTIES PUBLIC_HEADER "${PUBLIC_HEADER_FILES_BUILD}"
             PUBLIC_INCLUDE_DIRECTORY "${PUBLIC_INCLUDE_DIRECTORY}")

install(
  TARGETS rtapi_compat
  EXPORT machinekit_hal_rtapi_compat
  PUBLIC_HEADER DESTINATION "${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/runtime"
                COMPONENT MachinekitHAL_Library_RTAPI_Compat_Headers)

install(
  EXPORT machinekit_hal_rtapi_compat
  DESTINATION ${MACHINEKIT_HAL_CMAKE_PACKAGE_DIRECTORY}
  NAMESPACE "${MACHINEKIT_HAL_NAMESPACE}::"
  FILE "MachinekitHALRuntimeCompatTarget.cmake"
  COMPONENT MachinekitHAL_Library_RTAPI_Compat_Package_Exports)

export(
  EXPORT machinekit_hal_rtapi_compat
  NAMESPACE "${MACHINEKIT_HAL_NAMESPACE}::"
  FILE "${MACHINEKIT_HAL_CMAKE_PACKAGE_OUTPUT_DIRECTORY}/MachinekitHALRuntimeCompatTarget.cmake"
)

cpack_add_component(MachinekitHAL_Library_RTAPI_Compat_Headers
                    GROUP MachinekitHAL_Library_RTAPI_Compat_Development)
cpack_add_component(
  MachinekitHAL_Library_RTAPI_Compat_Package_Exports
  DEPENDS MachinekitHAL_Library_MKini_Headers
          MachinekitHAL_Library_Runtime_Config_Headers
  GROUP MachinekitHAL_Library_RTAPI_Compat_Development)

# Specification of artifacts placement in package tree
cpack_add_component_group(
  MachinekitHAL_Library_RTAPI_Compat_Development
  PARENT_GROUP MachinekitHAL_Package_Base_Libraries_Development)
