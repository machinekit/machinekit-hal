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

option(BUILD_HALSCOPE_MEMORY_LIBRARY "Define the HALscope memory library." TRUE)

if(BUILD_HALSCOPE_MEMORY_LIBRARY)
  add_library(halscope_memory INTERFACE)
  add_library(${MACHINEKIT_HAL_NAMESPACE}::halscope_memory ALIAS
              halscope_memory)

  set(PUBLIC_INCLUDE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include")

  set(PUBLIC_HEADER_FILES_BUILD
      ${PUBLIC_INCLUDE_DIRECTORY}/halscope_memory/scope_shm.h)

  set(PUBLIC_HEADER_FILES_INSTALL
      ${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/halscope_memory/scope_shm.h)

  target_sources(
    halscope_memory
    INTERFACE "$<BUILD_INTERFACE:${PUBLIC_HEADER_FILES_BUILD}>"
              "$<INSTALL_INTERFACE:${PUBLIC_HEADER_FILES_INSTALL}>")

  target_include_directories(
    halscope_memory
    INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
              $<INSTALL_INTERFACE:${MACHINEKIT_HAL_INTERFACE_DIRECTORY}>)

  set_target_properties(
    halscope_memory
    PROPERTIES PUBLIC_HEADER "${PUBLIC_HEADER_FILES_BUILD}"
               PUBLIC_INCLUDE_DIRECTORY "${PUBLIC_INCLUDE_DIRECTORY}")

  install(
    TARGETS halscope_memory
    EXPORT machinekit_hal_halscope_memory
    PUBLIC_HEADER
      DESTINATION "${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/halscope_memory"
      COMPONENT MachinekitHAL_Library_HALscope_Memory_Headers)

  install(
    EXPORT machinekit_hal_halscope_memory
    DESTINATION ${MACHINEKIT_HAL_CMAKE_PACKAGE_DIRECTORY}
    NAMESPACE "${MACHINEKIT_HAL_NAMESPACE}::"
    FILE "MachinekitHALHALScopeMemoryTarget.cmake"
    COMPONENT MachinekitHAL_Library_HALscope_Memory_Package_Exports)

  export(
    EXPORT machinekit_hal_halscope_memory
    NAMESPACE "${MACHINEKIT_HAL_NAMESPACE}::"
    FILE "${MACHINEKIT_HAL_CMAKE_PACKAGE_OUTPUT_DIRECTORY}/MachinekitHALHALScopeMemoryTarget.cmake"
  )

  cpack_add_component(MachinekitHAL_Library_HALscope_Memory_Headers
                      GROUP MachinekitHAL_Library_HALscope_Memory_Development)
  cpack_add_component(
    MachinekitHAL_Library_HALscope_Memory_Package_Exports
    DEPENDS MachinekitHAL_Library_HALscope_Memory_Headers
    GROUP MachinekitHAL_Library_HALscope_Memory_Development)

  # Specification of artifacts placement in package tree
  cpack_add_component_group(
    MachinekitHAL_Library_HALscope_Memory_Development
    PARENT_GROUP MachinekitHAL_Package_Base_Libraries_Development)
endif()
