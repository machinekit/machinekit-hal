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

option(BUILD_REALTIME_EXECUTABLE "Build the Realtime executable." TRUE)

if(BUILD_REALTIME_EXECUTABLE)
  set(SOURCE_FILE src/realtime.in)

  # Artifacts used when running from BINARY tree
  set(REALTIME_RTAPI_INI
      "${MACHINEKIT_HAL_SYSTEM_CONFIG_OUTPUT_DIRECTORY}/rtapi.ini") # TODO:
                                                                    # Change to
                                                                    # target
                                                                    # property
  set(REALTIME_MACHINEKIT_INI
      "${MACHINEKIT_HAL_SYSTEM_CONFIG_OUTPUT_DIRECTORY}/machinekit.ini"
  )# TODO: Change to target property
  set(REALTIME_INIVAR "$<TARGET_FILE:inivar>")
  set(REALTIME_RTAPI_MSGD "$<TARGET_FILE:msgd>")
  set(REALTIME_RTAPI_APP "$<TARGET_FILE:rtapi_app>")

  configure_file(${SOURCE_FILE} build/realtime.configure @ONLY
                 NEWLINE_STYLE UNIX)

  file(
    GENERATE
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/build/$<CONFIG>/realtime
    INPUT ${CMAKE_CURRENT_BINARY_DIR}/build/realtime.configure
    FILE_PERMISSIONS
    OWNER_READ
    OWNER_EXECUTE
    GROUP_READ
    GROUP_EXECUTE
    WORLD_READ
    WORLD_EXECUTE
    NEWLINE_STYLE
    UNIX)

  add_custom_target(
    realtime ALL
    COMMENT "Creating the Realtime executable"
    COMMAND
      ${CMAKE_COMMAND} "-E" "copy_if_different"
      "${CMAKE_CURRENT_BINARY_DIR}/build/$<CONFIG>/realtime"
      "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
    DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/build/$<CONFIG>/realtime")

  # Artifacts used when running from installation

  set(REALTIME_RTAPI_INI
      "${MACHINEKIT_HAL_SYSTEM_CONFIG_FULL_INSTALL_DIRECTORY}/rtapi.ini") # TODO:
                                                                    # Change to
                                                                    # target
                                                                    # property
  set(REALTIME_MACHINEKIT_INI
      "${MACHINEKIT_HAL_SYSTEM_CONFIG_FULL_INSTALL_DIRECTORY}/machinekit.ini"
  )# TODO: Change to target property
  set(REALTIME_INIVAR "${MACHINEKIT_HAL_EXECUTABLE_FULL_INSTALL_DIRECTORY}/$<TARGET_FILE_NAME:inivar>")
  set(REALTIME_RTAPI_MSGD "${MACHINEKIT_HAL_INTERNAL_EXECUTABLE_FULL_INSTALL_DIRECTORY}/$<TARGET_FILE_NAME:msgd>")
  set(REALTIME_RTAPI_APP "${MACHINEKIT_HAL_INTERNAL_EXECUTABLE_FULL_INSTALL_DIRECTORY}/$<TARGET_FILE_NAME:rtapi_app>")

  configure_file(${SOURCE_FILE} install/realtime.configure @ONLY
                 NEWLINE_STYLE UNIX)

  file(
    GENERATE
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/install/$<CONFIG>/realtime
    INPUT ${CMAKE_CURRENT_BINARY_DIR}/install/realtime.configure
    FILE_PERMISSIONS
    OWNER_READ
    OWNER_EXECUTE
    GROUP_READ
    GROUP_EXECUTE
    WORLD_READ
    WORLD_EXECUTE
    NEWLINE_STYLE
    UNIX)
endif()
