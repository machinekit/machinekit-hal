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

option(BUILD_SYSLOG_ASYNC_LIBRARY "Built the Syslog async library." TRUE)

if(BUILD_SYSLOG_ASYNC_LIBRARY)
  add_library(syslog_async SHARED)
  add_library(${MACHINEKIT_HAL_NAMESPACE}::syslog_async ALIAS syslog_async)

  set(SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/syslog_async.c)

  set(PUBLIC_INCLUDE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include")

  set(PUBLIC_HEADER_FILES_BUILD
      ${PUBLIC_INCLUDE_DIRECTORY}/syslog_async/syslog_async.h)

  set(PUBLIC_HEADER_FILES_INSTALL
      ${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/syslog_async/syslog_async.h)

  target_include_directories(
    syslog_async
    PUBLIC $<BUILD_INTERFACE:${PUBLIC_INCLUDE_DIRECTORY}>
           $<INSTALL_INTERFACE:${MACHINEKIT_HAL_INTERFACE_DIRECTORY}>)

  target_sources(
    syslog_async #[[PUBLIC $<BUILD_INTERFACE:${PUBLIC_HEADER_FILES_BUILD}>
           $<INSTALL_INTERFACE:${PUBLIC_HEADER_FILES_INSTALL}>]]
    PRIVATE ${SOURCE_FILES})

  set_target_properties(
    syslog_async
    PROPERTIES SOVERSION 0
               PREFIX "libmkh"
               VERSION ${CMAKE_PROJECT_VERSION}
               PUBLIC_HEADER "${PUBLIC_HEADER_FILES_BUILD}"
               POSITION_INDEPENDENT_CODE TRUE
               PUBLIC_INCLUDE_DIRECTORY "${PUBLIC_INCLUDE_DIRECTORY}")
  install(
    TARGETS syslog_async
    EXPORT machinekit_hal_syslog_async
    LIBRARY DESTINATION "${MACHINEKIT_HAL_LIBRARY_DIRECTORY}"
            COMPONENT MachinekitHAL_Library_SysLog_Async_Libraries
            NAMELINK_COMPONENT MachinekitHAL_Library_SysLog_Async_Namelinks
    PUBLIC_HEADER
      DESTINATION "${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/syslog_async"
      COMPONENT MachinekitHAL_Library_SysLog_Async_Headers)

  install(
    EXPORT machinekit_hal_syslog_async
    DESTINATION ${MACHINEKIT_HAL_CMAKE_PACKAGE_DIRECTORY}
    NAMESPACE "${MACHINEKIT_HAL_NAMESPACE}::"
    FILE "MachinekitHALSysLogAsyncTarget.cmake"
    COMPONENT MachinekitHAL_Library_SysLog_Async_Package_Exports)

  export(
    EXPORT machinekit_hal_syslog_async
    NAMESPACE "${MACHINEKIT_HAL_NAMESPACE}::"
    FILE "${MACHINEKIT_HAL_CMAKE_PACKAGE_OUTPUT_DIRECTORY}/MachinekitHALSysLogAsyncTarget.cmake"
  )

  cpack_add_component(MachinekitHAL_Library_SysLog_Async_Libraries
                      GROUP MachinekitHAL_Library_SysLog_Async)
  cpack_add_component(
    MachinekitHAL_Library_SysLog_Async_Namelinks
    GROUP MachinekitHAL_Library_SysLog_Async_Development
    DEPENDS MachinekitHAL_Library_SysLog_Async_Libraries)
  cpack_add_component(
    MachinekitHAL_Library_SysLog_Async_Headers
    GROUP MachinekitHAL_Library_SysLog_Async_Development
    DEPENDS MachinekitHAL_Library_SysLog_Async_Libraries)
  cpack_add_component(
    MachinekitHAL_Library_SysLog_Async_Package_Exports
    GROUP MachinekitHAL_Library_SysLog_Async_Development
    DEPENDS MachinekitHAL_Library_SysLog_Async_Libraries
            MachinekitHAL_Library_SysLog_Async_Headers
            MachinekitHAL_Library_SysLog_Async_Namelinks)

  # Specification of artifacts placement in package tree
  cpack_add_component_group(MachinekitHAL_Library_SysLog_Async
                            PARENT_GROUP MachinekitHAL_Package_Base_Libraries)
  cpack_add_component_group(
    MachinekitHAL_Library_SysLog_Async_Development
    PARENT_GROUP MachinekitHAL_Package_Base_Libraries_Development)
endif()
