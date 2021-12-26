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

if(CMAKE_SYSTEM_PROCESSOR STREQUAL "arm")
  option(BUILD_PRU_DEFINES_LIBRARY "Built the PRU Defines library." TRUE)

  if(BUILD_PRU_DEFINES_LIBRARY)
    add_library(pru_defines INTERFACE)

    set(PUBLIC_INCLUDE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include")

    set(PUBLIC_HEADER_FILES_BUILD ${PUBLIC_INCLUDE_DIRECTORY}/pru_defines/pru.h)

    set(PUBLIC_HEADER_FILES_INSTALL
        ${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/pru_defines/pru.h)

    # TODO: How to integrate the 'pru_macros.hp' file?

    target_sources(
      pru_defines
      INTERFACE "$<BUILD_INTERFACE:${PUBLIC_HEADER_FILES_BUILD}>"
                "$<INSTALL_INTERFACE:${PUBLIC_HEADER_FILES_INSTALL}>")

    target_include_directories(
      pru_defines
      INTERFACE $<BUILD_INTERFACE:${PUBLIC_INCLUDE_DIRECTORY}>
                $<INSTALL_INTERFACE:${MACHINEKIT_HAL_INTERFACE_DIRECTORY}>)

    set_target_properties(
      pru_defines
      PROPERTIES PUBLIC_HEADER "${PUBLIC_HEADER_FILES_BUILD}"
                 PUBLIC_INCLUDE_DIRECTORY "${PUBLIC_INCLUDE_DIRECTORY}")

    install(
      TARGETS pru_defines
      EXPORT machinekit_hal_pru_defines
      PUBLIC_HEADER
        DESTINATION "${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/pru_defines"
        COMPONENT MachinekitHAL_Library_PRU_Defines_Headers)

    install(
      EXPORT machinekit_hal_pru_defines
      DESTINATION ${MACHINEKIT_HAL_CMAKE_PACKAGE_DIRECTORY}
      NAMESPACE "${MACHINEKIT_HAL_NAMESPACE}::"
      FILE "MachinekitHALPRUDefinesTarget.cmake"
      COMPONENT MachinekitHAL_Library_PRU_Defines_Package_Exports)

    export(
      EXPORT machinekit_hal_pru_defines
      NAMESPACE "${MACHINEKIT_HAL_NAMESPACE}::"
      FILE "${MACHINEKIT_HAL_CMAKE_PACKAGE_OUTPUT_DIRECTORY}/MachinekitHALPRUDefinesTarget.cmake"
    )

    cpack_add_component(MachinekitHAL_Library_PRU_Defines_Headers
                        GROUP MachinekitHAL_Library_PRU_Defines_Development)
    cpack_add_component(
      MachinekitHAL_Library_PRU_Defines_Package_Exports
      DEPENDS MachinekitHAL_Library_PRU_Defines_Headers
      GROUP MachinekitHAL_Library_PRU_Defines_Development)

    # Specification of artifacts placement in package tree
    cpack_add_component_group(
      MachinekitHAL_Library_PRU_Defines_Development
      PARENT_GROUP MachinekitHAL_Package_Base_Libraries_Development)
  endif()
endif()
