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

option(BUILD_HALRUN_EXECUTABLE "Build the HALrun executable." TRUE)

if(BUILD_HALRUN_EXECUTABLE)
  configure_file(
    src/halrun.in
    src/halrun
    FILE_PERMISSIONS
    OWNER_READ
    OWNER_EXECUTE
    GROUP_READ
    GROUP_EXECUTE
    WORLD_READ
    WORLD_EXECUTE
    @ONLY
    NEWLINE_STYLE UNIX)

  add_custom_target(
    halrun ALL
    COMMENT "Creating the HALrun executable"
    COMMAND
      ${CMAKE_COMMAND} "-E" "copy_if_different"
      "${CMAKE_CURRENT_BINARY_DIR}/src/halrun"
      "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
    DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/src/halrun")
endif()
