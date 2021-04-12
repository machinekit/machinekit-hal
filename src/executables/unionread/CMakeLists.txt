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

option(BUILD_UNIONREAD_EXECUTABLE "Build the unionread executable." FALSE)

if(BUILD_UNIONREAD_EXECUTABLE)
add_executable(unionread)

set(SOURCE_FILES src/unionread.c)

target_sources(unionread PRIVATE ${SOURCE_FILES})

target_include_directories(unionread PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

target_link_libraries(unionread PRIVATE protobuf-nanopb-static machinetalk)

target_compile_definitions(unionread PRIVATE "PB_FIELD_32BIT")

target_compile_options(unionread PRIVATE "-O3")
endif()
