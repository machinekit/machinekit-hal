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

option(BUILD_HOSTMOT2_BITFILE_LIBRARY "Define the Hostmot2 Bitfile library."
       TRUE)

if(BUILD_HOSTMOT2_BITFILE_LIBRARY)
  add_library(hostmot2_bitfile SHARED)
  add_library(${MACHINEKIT_HAL_NAMESPACE}::hostmot2_bitfile ALIAS
              hostmot2_bitfile)

  set(SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/bitfile.c)

  set(PUBLIC_INCLUDE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include")

  set(PUBLIC_HEADER_FILES ${PUBLIC_INCLUDE_DIRECTORY}/hostmot2/bitfile.h)

  target_sources(
    hostmot2_bitfile
    PUBLIC
      $<BUILD_INTERFACE:${PUBLIC_HEADER_FILES}>
      $<INSTALL_INTERFACE:${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/hostmot2/bitfile.h>
    PRIVATE ${SOURCE_FILES})

  target_include_directories(
    hostmot2_bitfile
    PUBLIC $<BUILD_INTERFACE:${PUBLIC_INCLUDE_DIRECTORY}>
           $<INSTALL_INTERFACE:${MACHINEKIT_HAL_INTERFACE_DIRECTORY}>)

  target_link_libraries(
    hostmot2_bitfile
    PUBLIC user_pci
    PRIVATE managed_runtime managed_hal runtime_math hostmot2_api)

  set_target_properties(
    hostmot2_bitfile
    PROPERTIES SOVERSION 0
               PREFIX "libmkh"
               VERSION ${CMAKE_PROJECT_VERSION}
               PUBLIC_HEADER "${PUBLIC_HEADER_FILES}"
               PUBLIC_INCLUDE_DIRECTORY "${PUBLIC_INCLUDE_DIRECTORY}")

  install(
    TARGETS hostmot2_bitfile
    EXPORT machinekit_hal_hostmot2_bitfile
    LIBRARY DESTINATION "${MACHINEKIT_HAL_LIBRARY_DIRECTORY}"
            COMPONENT MachinekitHAL_Library_Hostmot_Bitfile_Libraries
            NAMELINK_COMPONENT MachinekitHAL_Library_Hostmot_Bitfile_Namelinks
    PUBLIC_HEADER DESTINATION "${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/hostmot2"
                  COMPONENT MachinekitHAL_Library_Hostmot_Bitfile_Headers)

  install(
    EXPORT machinekit_hal_hostmot2_bitfile
    DESTINATION ${MACHINEKIT_HAL_CMAKE_PACKAGE_DIRECTORY}
    NAMESPACE "${MACHINEKIT_HAL_NAMESPACE}::"
    FILE "MachinekitHALHostMot2BitfileTarget.cmake"
    COMPONENT MachinekitHAL_Library_Hostmot_Bitfile_Package_Exports)

  export(
    EXPORT machinekit_hal_hostmot2_bitfile
    NAMESPACE "${MACHINEKIT_HAL_NAMESPACE}::"
    FILE "${MACHINEKIT_HAL_CMAKE_PACKAGE_OUTPUT_DIRECTORY}/MachinekitHALHostMot2BitfileTarget.cmake"
  )

  cpack_add_component(
    MachinekitHAL_Library_Hostmot_Bitfile_Libraries
    GROUP MachinekitHAL_Library_Hostmot_Bitfile
    DEPENDS MachinekitHAL_Library_Runtime_Math_Libraries
            MachinekitHAL_Library_User_PCI_Libraries)
  cpack_add_component(
    MachinekitHAL_Library_Hostmot_Bitfile_Namelinks
    GROUP MachinekitHAL_Library_Hostmot_Bitfile_Development
    DEPENDS MachinekitHAL_Library_Hostmot_Bitfile_Libraries
            MachinekitHAL_Library_HAL_Command_Libraries)
  cpack_add_component(
    MachinekitHAL_Library_Hostmot_Bitfile_Headers
    GROUP MachinekitHAL_Library_Hostmot_Bitfile_Development
    DEPENDS MachinekitHAL_Library_Hostmot_Bitfile_Libraries)
  cpack_add_component(
    MachinekitHAL_Library_Hostmot_Bitfile_Package_Exports
    GROUP MachinekitHAL_Library_Hostmot_Bitfile_Development
    DEPENDS MachinekitHAL_Library_Hostmot_Bitfile_Libraries
            MachinekitHAL_Library_Hostmot_Bitfile_Headers
            MachinekitHAL_Library_Hostmot_Bitfile_Namelinks)

  # Specification of artifacts placement in package tree
  cpack_add_component_group(MachinekitHAL_Library_Hostmot_Bitfile
                            PARENT_GROUP MachinekitHAL_Package_Base_Libraries)
  cpack_add_component_group(
    MachinekitHAL_Library_Hostmot_Bitfile_Development
    PARENT_GROUP MachinekitHAL_Package_Base_Libraries_Development)
endif()
