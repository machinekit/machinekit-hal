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

option(BUILD_HAL_COMMAND_LIBRARY "Built the HAL command library." TRUE)

if(BUILD_HAL_COMMAND_LIBRARY)
  add_library(hal_command SHARED)
  add_library(${MACHINEKIT_HAL_NAMESPACE}::hal_command ALIAS hal_command)

  set(SOURCE_FILES
      ${CMAKE_CURRENT_SOURCE_DIR}/src/halcmd_commands.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/halcmd.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/halcmd_rtapiapp.cc)
  set(PUBLIC_HEADER_FILES
      ${CMAKE_CURRENT_SOURCE_DIR}/include/hal_command/halcmd_commands.h
      ${CMAKE_CURRENT_SOURCE_DIR}/include/hal_command/halcmd.h
      ${CMAKE_CURRENT_SOURCE_DIR}/include/hal_command/halcmd_rtapiapp.h)

  target_include_directories(
    hal_command
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
           $<INSTALL_INTERFACE:"${MACHINEKIT_HAL_INTERFACE_DIRECTORY}")

  target_sources(hal_command PRIVATE ${SOURCE_FILES})

  pkg_check_modules(CZMQ "libczmq" REQUIRED IMPORTED_TARGET)

  # TODO: How come there is no dependency on HAL? What is transitively depending
  # it?
  target_link_libraries(
    hal_command
    PRIVATE runtime_config
            runtime_api
            runtime
            hal_api
            hal
            machinetalk_proto_c
            runtime_math
            runtime_memory_api
            mkini
            machinetalk
            PkgConfig::CZMQ)

  target_compile_definitions(hal_command PRIVATE "ULAPI")

  set_target_properties(
    hal_command
    PROPERTIES SOVERSION 0
               PREFIX "libmkh"
               VERSION ${CMAKE_PROJECT_VERSION}
               PUBLIC_HEADER "${PUBLIC_HEADER_FILES}"
               POSITION_INDEPENDENT_CODE TRUE)

  install(
    TARGETS hal_command
    EXPORT machinekit_hal_hal_command
    LIBRARY DESTINATION "${MACHINEKIT_HAL_LIBRARY_DIRECTORY}"
            COMPONENT MachinekitHAL_Library_HAL_Command_Libraries
            NAMELINK_COMPONENT MachinekitHAL_Library_HAL_Command_Namelinks
    PUBLIC_HEADER DESTINATION "${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/hal_command"
                  COMPONENT MachinekitHAL_Library_HAL_Command_Headers)

  cpack_add_component(
    MachinekitHAL_Library_HAL_Command_Libraries
    GROUP MachinekitHAL_Library_HAL_Command
    DEPENDS MachinekitHAL_Library_Proto_Libraries
            MachinekitHAL_Library_Runtime_Math_Libraries
            MachinekitHAL_Library_Runtime_Memory_API_Libraries
            MachinekitHAL_Library_MKini_Libraries
            MachinekitHAL_Library_Machinetalk_Libraries)
  cpack_add_component(
    MachinekitHAL_Library_HAL_Command_Namelinks
    GROUP MachinekitHAL_Library_HAL_Command_Development
    DEPENDS MachinekitHAL_Library_HAL_Command_Libraries)
  cpack_add_component(
    MachinekitHAL_Library_HAL_Command_Headers
    GROUP MachinekitHAL_Library_HAL_Command_Development
    DEPENDS MachinekitHAL_Library_HAL_Command_Libraries)

  # Specification of artifacts placement in package tree
  cpack_add_component_group(MachinekitHAL_Library_HAL_Command
                            PARENT_GROUP MachinekitHAL_Package_Base_Libraries)
  cpack_add_component_group(
    MachinekitHAL_Library_HAL_Command_Development
    PARENT_GROUP MachinekitHAL_Package_Base_Libraries_Development)
endif()
