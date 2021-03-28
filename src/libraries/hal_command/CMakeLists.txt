#####################################################################
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
######################################################################

add_library(hal_command SHARED)
add_library(${MACHINEKIT_HAL_NAMESPACE}::hal_command ALIAS hal_command)

set(SOURCE_FILES
    src/halcmd_commands.c
    src/halcmd.c
    src/halcmd_rtapiapp.cc)

target_include_directories(
    hal_command PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:"${MACHINEKIT_HAL_INTERFACE_DIRECTORY}")

target_sources(hal_command PRIVATE ${SOURCE_FILES})

# TODO: Needed just because of the config.h??
target_link_libraries(hal_command PRIVATE runtime_api hal_api)

target_compile_definitions(hal_command PRIVATE "ULAPI")

set_target_properties(
    hal_command PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${MACHINEKIT_HAL_LIBRARY_DIRECTORY}"
    PUBLIC_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/include/halcmd_commands.h"
    PUBLIC_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/include/halcmd_rtapiapp.h"
    PUBLIC_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/include/halcmd.h"
    )