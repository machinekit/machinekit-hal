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

option(BUILD_SYSLOG_ASYNC_LIBRARY "Built the Syslog async library." TRUE)

if(BUILD_SYSLOG_ASYNC_LIBRARY)
add_library(syslog_async SHARED)
add_library(${MACHINEKIT_HAL_NAMESPACE}::syslog_async ALIAS syslog_async)

set(HEADER_FILES include/syslog_async.h)
set(SOURCE_FILES src/syslog_async.c)

target_include_directories(
  syslog_async PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${MACHINEKIT_HAL_INTERFACE_DIRECTORY}>)

target_sources(syslog_async PRIVATE ${SOURCE_FILES})

set_target_properties(
  syslog_async
    PROPERTIES 
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_INSTALL_LIBDIR}"
        POSITION_INDEPENDENT_CODE TRUE)
endif()
