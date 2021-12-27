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

option(BUILD_FIX_UUID_EXECUTABLE "Build the Fix UUID executable." TRUE)

if(BUILD_FIX_UUID_EXECUTABLE)
  configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/src/fix-uuid
    ${CMAKE_CURRENT_BINARY_DIR}/src/fix-uuid
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
    fix_uuid ALL
    COMMENT "Creating the Fix UUID executable"
    COMMAND
      ${CMAKE_COMMAND} "-E" "copy_if_different"
      "${CMAKE_CURRENT_BINARY_DIR}/src/fix-uuid"
      "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
    DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/src/fix-uuid")

  install(
    PROGRAMS "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/fix-uuid"
    DESTINATION "${MACHINEKIT_HAL_EXECUTABLE_DIRECTORY}"
    COMPONENT MachinekitHAL_Executable_Fix_UUID_Binaries)

  cpack_add_component(MachinekitHAL_Executable_Fix_UUID_Binaries
                      GROUP MachinekitHAL_Executable_Fix_UUID)

  # Specification of artifacts placement in package tree
  cpack_add_component_group(MachinekitHAL_Executable_Fix_UUID
                            PARENT_GROUP MachinekitHAL_Package_Base_Executables)
endif()
