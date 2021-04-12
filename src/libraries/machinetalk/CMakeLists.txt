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

option(BUILD_MACHINETALK_LIBRARY "Built the Machinetalk library." TRUE)

if(BUILD_MACHINETALK_LIBRARY)
add_library(machinetalk SHARED)
add_library(${MACHINEKIT_HAL_NAMESPACE}::machinetalk ALIAS machinetalk)

set(SOURCE_FILES src/czmq-watch.c
    src/inihelp.cc
    src/json2pb.cc
    src/ll_zeroconf_register.cc
    src/mk_backtrace.c
    src/mk_service.cc
    src/mk_zeroconf.cc
    src/pbutil.cc
    src/setup_signals.c
    src/zeroconf_resolve.cc)

target_sources(machinetalk PRIVATE ${SOURCE_FILES})

pkg_check_modules(AVAHICLIENT "avahi-client" REQUIRED IMPORTED_TARGET)
pkg_check_modules(LIBCZMQ "libczmq" REQUIRED IMPORTED_TARGET)
pkg_check_modules(JANSSON "jansson" REQUIRED IMPORTED_TARGET)

target_link_libraries(machinetalk 
PUBLIC 
machinetalk_proto_cc
nanopb_interface
PkgConfig::AVAHICLIENT
PkgConfig::LIBCZMQ
PkgConfig::JANSSON
PRIVATE 
runtime_api 
hal_api 
mkini 
syslog_async
)

target_include_directories(
    machinetalk PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${MACHINEKIT_HAL_INTERFACE_DIRECTORY})

set_target_properties(
    machinetalk
    PROPERTIES 
        RUNTIME_OUTPUT_DIRECTORY "${MACHINEKIT_HAL_LIBRARY_DIRECTORY}"
        POSITION_INDEPENDENT_CODE TRUE)
endif()
