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
  option(BUILD_RASPI_CPUINFO_LIBRARY "Build the Raspberry Pi CPUInfo library."
         TRUE)

  if(BUILD_RASPI_CPUINFO_LIBRARY)
    add_library(raspi_cpuinfo STATIC)
    add_library(${MACHINEKIT_HAL_NAMESPACE}::raspi_cpuinfo ALIAS raspi_cpuinfo)

    set(SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/cpuinfo.c)

    set(PUBLIC_INCLUDE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include")

    set(PUBLIC_HEADER_FILES_BUILD
        ${PUBLIC_INCLUDE_DIRECTORY}/raspi_cpuinfo/cpuinfo.h)

    set(PUBLIC_HEADER_FILES_INSTALL
        ${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/raspi_cpuinfo/cpuinfo.h)

    target_sources(
      raspi_cpuinfo
      PUBLIC "$<BUILD_INTERFACE:${PUBLIC_HEADER_FILES_BUILD}>"
             "$<INSTALL_INTERFACE:${PUBLIC_HEADER_FILES_INSTALL}>"
      PRIVATE ${SOURCE_FILES})

    target_include_directories(
      raspi_cpuinfo
      PUBLIC $<BUILD_INTERFACE:${PUBLIC_INCLUDE_DIRECTORY}>
             $<INSTALL_INTERFACE:${MACHINEKIT_HAL_INTERFACE_DIRECTORY}>)

    set_target_properties(
      raspi_cpuinfo
      PROPERTIES PREFIX "libmkh"
                 PUBLIC_HEADER "${PUBLIC_HEADER_FILES_BUILD}"
                 POSITION_INDEPENDENT_CODE TRUE
                 PUBLIC_INCLUDE_DIRECTORY "${PUBLIC_INCLUDE_DIRECTORY}")
    install(
      TARGETS raspi_cpuinfo
      EXPORT machinekit_hal_raspi_cpuinfo
      ARCHIVE DESTINATION "${MACHINEKIT_HAL_LIBRARY_DIRECTORY}"
              COMPONENT MachinekitHAL_Library_RasPi_CPUInfo_Archives
      PUBLIC_HEADER
        DESTINATION "${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/raspi_cpuinfo"
        COMPONENT MachinekitHAL_Library_RasPi_CPUInfo_Headers)

    install(
      EXPORT machinekit_hal_raspi_cpuinfo
      DESTINATION ${MACHINEKIT_HAL_CMAKE_PACKAGE_DIRECTORY}
      NAMESPACE "${MACHINEKIT_HAL_NAMESPACE}::"
      FILE "MachinekitHALRaspiCPUInfoTarget.cmake"
      COMPONENT MachinekitHAL_Library_RasPi_CPUInfo_Package_Exports)

    export(
      EXPORT machinekit_hal_raspi_cpuinfo
      NAMESPACE "${MACHINEKIT_HAL_NAMESPACE}::"
      FILE "${MACHINEKIT_HAL_CMAKE_PACKAGE_OUTPUT_DIRECTORY}/MachinekitHALRaspiCPUInfoTarget.cmake"
    )

    cpack_add_component(MachinekitHAL_Library_RasPi_CPUInfo_Archives
                        GROUP MachinekitHAL_Library_RasPi_CPUInfo_Development)
    cpack_add_component(
      MachinekitHAL_Library_RasPi_CPUInfo_Headers
      GROUP MachinekitHAL_Library_RasPi_CPUInfo_Development
      DEPENDS MachinekitHAL_Library_RasPi_CPUInfo_Archives)
    cpack_add_component(
      MachinekitHAL_Library_RasPi_CPUInfo_Package_Exports
      GROUP MachinekitHAL_Library_RasPi_CPUInfo_Development
      DEPENDS MachinekitHAL_Library_RasPi_CPUInfo_Archives
              MachinekitHAL_Library_RasPi_CPUInfo_Headers)

    # Specification of artifacts placement in package tree
    cpack_add_component_group(
      MachinekitHAL_Library_RasPi_CPUInfo_Development
      PARENT_GROUP MachinekitHAL_Package_Base_Libraries_Development)
  endif()
endif()
