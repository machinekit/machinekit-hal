# ##############################################################################
# Description:  CMakeLists.txt
#
# This file, 'CMakeLists.txt', implements build system rules for Machinekit-HAL
# project
#
# Copyright (C) 2021    Jakub Fišer  <jakub DOT fiser AT eryaf DOT com>
#
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU Lesser General Public License as published by the Free
# Software Foundation; either version 2.1 of the License, or (at your option)
# any later version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
# details.
#
# You should have received a copy of the GNU Lesser General Public License along
# with this library; if not, write to the Free Software Foundation, Inc., 51
# Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
#
# ##############################################################################

option(BUILD_SAMSTR_MEMDEFS_LIBRARY "Built the SamStr MemDefs library." TRUE)

if(BUILD_SAMSTR_MEMDEFS_LIBRARY)
  add_library(samstr_memdefs INTERFACE)

  set(PUBLIC_INCLUDE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include")

  set(PUBLIC_HEADER_FILES
      ${PUBLIC_INCLUDE_DIRECTORY}/samstr_memdefs/samstr_memdefs.h)

  target_include_directories(
    samstr_memdefs
    INTERFACE $<BUILD_INTERFACE:${PUBLIC_INCLUDE_DIRECTORY}>
              $<INSTALL_INTERFACE:${MACHINEKIT_HAL_INTERFACE_DIRECTORY}>)

  set_target_properties(
    samstr_memdefs
    PROPERTIES PUBLIC_HEADER ${PUBLIC_HEADER_FILES}
               PUBLIC_INCLUDE_DIRECTORY "${PUBLIC_INCLUDE_DIRECTORY}")

  target_link_libraries(samstr_memdefs INTERFACE runtime_api)

  install(
    TARGETS samstr_memdefs
    EXPORT machinekit_hal_samstr_memdefs
    PUBLIC_HEADER
      DESTINATION "${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/samstr_memdefs"
      COMPONENT MachinekitHAL_Library_SamStr_MemDefs_Headers)

  install(
    EXPORT machinekit_hal_samstr_memdefs
    DESTINATION ${MACHINEKIT_HAL_CMAKE_PACKAGE_DIRECTORY}
    NAMESPACE "${MACHINEKIT_HAL_NAMESPACE}::"
    FILE "MachinekitHALSamStrMemDefsTarget.cmake"
    COMPONENT MachinekitHAL_Library_SamStr_MemDefs_Package_Exports)

  export(
    EXPORT machinekit_hal_samstr_memdefs
    NAMESPACE "${MACHINEKIT_HAL_NAMESPACE}::"
    FILE "${MACHINEKIT_HAL_CMAKE_PACKAGE_OUTPUT_DIRECTORY}/MachinekitHALSamStrMemDefsTarget.cmake"
  )

  cpack_add_component(MachinekitHAL_Library_SamStr_MemDefs_Headers
                      GROUP MachinekitHAL_Library_SamStr_MemDefs_Development)
  cpack_add_component(
    MachinekitHAL_Library_SamStr_MemDefs_Package_Exports
    DEPENDS MachinekitHAL_Library_SamStr_MemDefs_Headers
    GROUP MachinekitHAL_Library_SamStr_MemDefs_Development)

  # Specification of artifacts placement in package tree
  cpack_add_component_group(
    MachinekitHAL_Library_SamStr_MemDefs_Development
    PARENT_GROUP MachinekitHAL_Package_Base_Libraries_Development)
endif()
