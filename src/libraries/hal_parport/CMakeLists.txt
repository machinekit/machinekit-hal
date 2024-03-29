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

if(NOT DEFINED SYS_IO_AVAILABLE)
  if(NOT DEFINED check_include_files)
    include(CheckIncludeFiles)
  endif()

  check_include_files("sys/io.h" SYS_IO_AVAILABLE)
endif()

if(SYS_IO_AVAILABLE)
  # Target clashes with the managed driver module 'hal_parport' TODO: Think a
  # better/more descriptive name
  add_library(rtapi_parport INTERFACE)

  set(PUBLIC_INCLUDE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include")

  set(PUBLIC_HEADER_FILES_BUILD
      ${PUBLIC_INCLUDE_DIRECTORY}/hal_parport/hal_parport.h)

  set(PUBLIC_HEADER_FILES_INSTALL
      ${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/hal_parport/hal_parport.h)

  target_sources(
    rtapi_parport
    INTERFACE "$<BUILD_INTERFACE:${PUBLIC_HEADER_FILES_BUILD}>"
              "$<INSTALL_INTERFACE:${PUBLIC_HEADER_FILES_INSTALL}>")

  target_include_directories(
    rtapi_parport
    INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
              $<INSTALL_INTERFACE:${MACHINEKIT_HAL_INTERFACE_DIRECTORY}>)

  set_target_properties(
    rtapi_parport
    PROPERTIES PUBLIC_HEADER "${PUBLIC_HEADER_FILES_BUILD}"
               PUBLIC_INCLUDE_DIRECTORY "${PUBLIC_INCLUDE_DIRECTORY}")

  install(
    TARGETS rtapi_parport
    EXPORT machinekit_hal_hal_parport
    PUBLIC_HEADER
      DESTINATION "${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/hal_parport"
      COMPONENT MachinekitHAL_Library_HAL_Parport_Headers)

  install(
    EXPORT machinekit_hal_hal_parport
    DESTINATION ${MACHINEKIT_HAL_CMAKE_PACKAGE_DIRECTORY}
    NAMESPACE "${MACHINEKIT_HAL_NAMESPACE}::"
    FILE "MachinekitHALHALParportTarget.cmake"
    COMPONENT MachinekitHAL_Library_HAL_Parport_Package_Exports)

  export(
    EXPORT machinekit_hal_hal_parport
    NAMESPACE "${MACHINEKIT_HAL_NAMESPACE}::"
    FILE "${MACHINEKIT_HAL_CMAKE_PACKAGE_OUTPUT_DIRECTORY}/MachinekitHALHALParportTarget.cmake"
  )

  cpack_add_component(MachinekitHAL_Library_HAL_Parport_Headers
                      GROUP MachinekitHAL_Library_HAL_Parport_Development)
  cpack_add_component(
    MachinekitHAL_Library_HAL_Parport_Package_Exports
    DEPENDS MachinekitHAL_Library_HAL_Parport_Headers
    GROUP MachinekitHAL_Library_HAL_Parport_Development)

  # Specification of artifacts placement in package tree
  cpack_add_component_group(
    MachinekitHAL_Library_HAL_Parport_Development
    PARENT_GROUP MachinekitHAL_Package_Base_Libraries_Development)
endif()
