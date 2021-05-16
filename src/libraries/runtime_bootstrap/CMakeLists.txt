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

add_library(runtime_config SHARED)
add_library(${MACHINEKIT_HAL_NAMESPACE}::runtime_config ALIAS runtime_config)

add_library(runtime_config-install SHARED)
add_library(${MACHINEKIT_HAL_NAMESPACE}::runtime_config-install ALIAS runtime_config-install)

set(RUNTIME_CONFIG include/config.h.in)
set(SOURCE_FILE src/runtime_config.c.in)

set(LINELEN "255")
set(ZMQIPC_FORMAT "ipc://%s/%d.%s.%s")
set(RUNDIR "/tmp")
set(MACHINEKIT_HAL_CONFIG_DIRECTORY "${MACHINEKIT_HAL_CONFIG_OUTPUT_DIRECTORY}")
set(MACHINEKIT_HAL_INI_FILE "${MACHINEKIT_HAL_CONFIG_OUTPUT_DIRECTORY}/machinekit.ini")

configure_file(${RUNTIME_CONFIG} ${CMAKE_CURRENT_BINARY_DIR}/build/runtime_config/include/runtime/config.h
    NO_SOURCE_PERMISSIONS 
    @ONLY NEWLINE_STYLE UNIX)
configure_file(${SOURCE_FILE} ${CMAKE_CURRENT_BINARY_DIR}/build/runtime_config/src/runtime_config.c NO_SOURCE_PERMISSIONS 
    @ONLY NEWLINE_STYLE UNIX)

set(MACHINEKIT_HAL_CONFIG_DIRECTORY "${CMAKE_INSTALL_FULL_SYSCONFDIR}/machinekit/hal")
set(MACHINEKIT_HAL_INI_FILE "${CMAKE_INSTALL_FULL_SYSCONFDIR}/machinekit/hal/machinekit.ini")
    
configure_file(${RUNTIME_CONFIG} ${CMAKE_CURRENT_BINARY_DIR}/install/runtime_config/include/runtime/config.h
        NO_SOURCE_PERMISSIONS 
        @ONLY NEWLINE_STYLE UNIX)
configure_file(${SOURCE_FILE} ${CMAKE_CURRENT_BINARY_DIR}/install/runtime_config/src/runtime_config.c NO_SOURCE_PERMISSIONS 
        @ONLY NEWLINE_STYLE UNIX)

target_sources(runtime_config PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/build/runtime_config/include/runtime/config.h
                              PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/build/runtime_config/src/runtime_config.c)

target_include_directories(runtime_config PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/build/runtime_config/include/runtime)

target_sources(runtime_config-install PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/install/runtime_config/include/runtime/config.h
                              PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/install/runtime_config/src/runtime_config.c)

target_include_directories(runtime_config-install PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/install/runtime_config/include/runtime)

set_target_properties(runtime_config PROPERTIES
    SOVERSION 0)

set_target_properties(runtime_config-install PROPERTIES
    SOVERSION 0)
