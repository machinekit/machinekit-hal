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

add_library(runtime_config SHARED)
add_library(${MACHINEKIT_HAL_NAMESPACE}::runtime_config ALIAS runtime_config)

add_library(runtime_config_binary_tree SHARED)

set(RUNTIME_CONFIG ${CMAKE_CURRENT_SOURCE_DIR}/include/runtime/config.h.in)
set(SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/runtime_config.c.in)

set(GIT_CONFIG_FILE ${CMAKE_CURRENT_SOURCE_DIR}/include/runtime/git_config.h.in)

set(PUBLIC_INCLUDE_DIRECTORY
    "${CMAKE_CURRENT_BINARY_DIR}/runtime_config/include")

set(PRE_CONFIGURE_FILE "${GIT_CONFIG_FILE}")
set(POST_CONFIGURE_FILE "${PUBLIC_INCLUDE_DIRECTORY}/runtime/git_config.h")
include(git_watcher)

set(LINELEN "255")
set(ZMQIPC_FORMAT "ipc://%s/%d.%s.%s")
set(RUNDIR "/tmp")
set(MACHINEKIT_HAL_CONFIG_DIRECTORY
    "${MACHINEKIT_HAL_SYSTEM_CONFIG_OUTPUT_DIRECTORY}")
set(MACHINEKIT_HAL_INI_FILE
    "${MACHINEKIT_HAL_SYSTEM_CONFIG_OUTPUT_DIRECTORY}/machinekit.ini")
set(MACHINEKIT_HAL_LOG_FILE
    "${MACHINEKIT_HAL_LOCAL_STATE_OUTPUT_DIRECTORY}/log/hal.log")
set(MACHINEKIT_HAL_BIN_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

configure_file(${RUNTIME_CONFIG} ${PUBLIC_INCLUDE_DIRECTORY}/runtime/config.h
               NO_SOURCE_PERMISSIONS @ONLY NEWLINE_STYLE UNIX)

configure_file(
  ${SOURCE_FILES}
  ${CMAKE_CURRENT_BINARY_DIR}/build/runtime_config/runtime_config.c.configure.binary_tree
  NO_SOURCE_PERMISSIONS
  @ONLY
  NEWLINE_STYLE UNIX)

file(
  GENERATE
  OUTPUT
    ${CMAKE_CURRENT_BINARY_DIR}/build/runtime_config/src/$<CONFIG>/runtime_config.c
  INPUT
    ${CMAKE_CURRENT_BINARY_DIR}/build/runtime_config/runtime_config.c.configure.binary_tree
  NO_SOURCE_PERMISSIONS NEWLINE_STYLE UNIX)

# TODO: Use the right variables to assemble the paths!
set(MACHINEKIT_HAL_CONFIG_DIRECTORY
    "${CMAKE_INSTALL_FULL_SYSCONFDIR}/machinekit/hal")
set(MACHINEKIT_HAL_INI_FILE
    "${CMAKE_INSTALL_FULL_SYSCONFDIR}/machinekit/hal/machinekit.ini")
set(MACHINEKIT_HAL_LOG_FILE "${CMAKE_INSTALL_FULL_LOCALSTATEDIR}/log/hal.log")
set(MACHINEKIT_HAL_BIN_DIRECTORY
    "${MACHINEKIT_HAL_EXECUTABLE_FULL_INSTALL_DIRECTORY}")

configure_file(
  ${SOURCE_FILES}
  ${CMAKE_CURRENT_BINARY_DIR}/install/runtime_config/runtime_config.c.configure
  NO_SOURCE_PERMISSIONS @ONLY NEWLINE_STYLE UNIX)

file(
  GENERATE
  OUTPUT
    ${CMAKE_CURRENT_BINARY_DIR}/install/runtime_config/src/$<CONFIG>/runtime_config.c
  INPUT
    ${CMAKE_CURRENT_BINARY_DIR}/install/runtime_config/runtime_config.c.configure
  NO_SOURCE_PERMISSIONS NEWLINE_STYLE UNIX)

target_sources(
  runtime_config_binary_tree
  PUBLIC "$<BUILD_INTERFACE:${PUBLIC_INCLUDE_DIRECTORY}/runtime/config.h>"
         "$<BUILD_INTERFACE:${POST_CONFIGURE_FILE}>"
  PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}/build/runtime_config/src/$<CONFIG>/runtime_config.c
)

target_include_directories(
  runtime_config_binary_tree
  PUBLIC $<BUILD_INTERFACE:${PUBLIC_INCLUDE_DIRECTORY}>)

target_sources(
  runtime_config
  PUBLIC
    "$<BUILD_INTERFACE:${PUBLIC_INCLUDE_DIRECTORY}/runtime/config.h>"
    "$<BUILD_INTERFACE:${POST_CONFIGURE_FILE}>"
    "$<INSTALL_INTERFACE:${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/runtime/config.h>"
  PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}/install/runtime_config/src/$<CONFIG>/runtime_config.c
)

target_include_directories(
  runtime_config
  PUBLIC $<BUILD_INTERFACE:${PUBLIC_INCLUDE_DIRECTORY}>
         $<INSTALL_INTERFACE:${MACHINEKIT_HAL_INTERFACE_DIRECTORY}>)

set_target_properties(
  runtime_config_binary_tree
  PROPERTIES SOVERSION 0
             PREFIX "libmkh"
             VERSION ${CMAKE_PROJECT_VERSION}
             PUBLIC_INCLUDE_DIRECTORY "${PUBLIC_BUILD_INCLUDE_DIRECTORY}"
             OUTPUT_NAME "runtime_config" # Needs to have the same name as
                                          # target 'runtime_config'
)
set_target_properties(
  runtime_config
  PROPERTIES SOVERSION 0
             PREFIX "libmkh"
             VERSION ${CMAKE_PROJECT_VERSION}
             LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/install"
             PUBLIC_HEADER "${PUBLIC_INCLUDE_DIRECTORY}/runtime/config.h"
             PUBLIC_INCLUDE_DIRECTORY
             "$<BUILD_INTERFACE:${PUBLIC_INSTALL_INCLUDE_DIRECTORY}>")
# "$<INSTALL_INTERFACE:${MACHINEKIT_HAL_INTERFACE_DIRECTORY}>")

add_dependencies(runtime_config_binary_tree check_git)
add_dependencies(runtime_config check_git)

configure_file(
  src/dot_envrc.in ${CMAKE_CURRENT_BINARY_DIR}/build/dot_envrc.configure @ONLY
  NEWLINE_STYLE UNIX FILE_PERMISSIONS OWNER_READ GROUP_READ WORLD_READ)

file(
  GENERATE
  OUTPUT ${MACHINEKIT_HAL_ARTIFACTS_MOUNTPOINT_DIRECTORY}/.envrc
  INPUT ${CMAKE_CURRENT_BINARY_DIR}/build/dot_envrc.configure
  NEWLINE_STYLE UNIX FILE_PERMISSIONS OWNER_READ GROUP_READ WORLD_READ)

install(
  TARGETS runtime_config
  EXPORT machinekit_hal_runtime_config
  LIBRARY DESTINATION "${MACHINEKIT_HAL_LIBRARY_DIRECTORY}"
          COMPONENT MachinekitHAL_Library_Runtime_Config_Libraries
          NAMELINK_COMPONENT MachinekitHAL_Library_Runtime_Config_Namelinks
  PUBLIC_HEADER DESTINATION "${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/runtime"
                COMPONENT MachinekitHAL_Library_Runtime_Config_Headers)

install(
  EXPORT machinekit_hal_runtime_config
  DESTINATION ${MACHINEKIT_HAL_CMAKE_PACKAGE_DIRECTORY}
  NAMESPACE "${MACHINEKIT_HAL_NAMESPACE}::"
  FILE "MachinekitHALRuntimeConfigTarget.cmake"
  COMPONENT MachinekitHAL_Library_Runtime_Config_Package_Exports)

export(
  # EXPORT machinekit_hal_runtime_config
  TARGETS runtime_config
  NAMESPACE "${MACHINEKIT_HAL_NAMESPACE}::"
  FILE "${MACHINEKIT_HAL_CMAKE_PACKAGE_OUTPUT_DIRECTORY}/MachinekitHALRuntimeConfigTarget.cmake"
)

export(
  # EXPORT machinekit_hal_runtime_config
  TARGETS runtime_config_binary_tree
  NAMESPACE "${MACHINEKIT_HAL_NAMESPACE}::"
  FILE "${CMAKE_CURRENT_BINARY_DIR}/MachinekitHALRuntimeConfigTarget.cmake")

cpack_add_component(MachinekitHAL_Library_Runtime_Config_Libraries
                    GROUP MachinekitHAL_Library_Runtime_Config)
cpack_add_component(
  MachinekitHAL_Library_Runtime_Config_Namelinks
  GROUP MachinekitHAL_Library_Runtime_Config_Development
  DEPENDS MachinekitHAL_Library_Runtime_Config_Libraries)
cpack_add_component(
  MachinekitHAL_Library_Runtime_Config_Headers
  GROUP MachinekitHAL_Library_Runtime_Config_Development
  DEPENDS MachinekitHAL_Library_Runtime_Config_Libraries)
cpack_add_component(
  MachinekitHAL_Library_Runtime_Config_Package_Exports
  GROUP MachinekitHAL_Library_Runtime_Config_Development
  DEPENDS MachinekitHAL_Library_Runtime_Config_Libraries
          MachinekitHAL_Library_Runtime_Config_Headers
          MachinekitHAL_Library_Runtime_Namelinks)

# Specification of artifacts placement in package tree
cpack_add_component_group(MachinekitHAL_Library_Runtime_Config
                          PARENT_GROUP MachinekitHAL_Package_Base_Libraries)
cpack_add_component_group(
  MachinekitHAL_Library_Runtime_Config_Development
  PARENT_GROUP MachinekitHAL_Package_Base_Libraries_Development)
