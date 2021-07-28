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
  option(BUILD_RASPI_CPUINFO_LIBRARY
         "Built the Raspberry Pi SPI HAL managed driver module." TRUE)

  if(BUILD_RASPI_CPUINFO_LIBRARY)
    add_library(raspi_cpuinfo STATIC)

    set(SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/cpuinfo.c)

    set(PUBLIC_HEADER_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/include/raspi_cpuinfo/cpuinfo.h)

    target_sources(raspi_cpuinfo PRIVATE ${SOURCE_FILES} ${PUBLIC_HEADER_FILES})

    target_include_directories(
      raspi_cpuinfo
      PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
             $<INSTALL_INTERFACE:${MACHINEKIT_HAL_INTERFACE_DIRECTORY}>)

    set_target_properties(
      raspi_cpuinfo PROPERTIES PUBLIC_HEADER "${PUBLIC_HEADER_FILES}"
                               POSITION_INDEPENDENT_CODE TRUE)
    install(
      TARGETS raspi_cpuinfo
      EXPORT machinekit_hal_raspi_cpuinfo
      LIBRARY DESTINATION "${MACHINEKIT_HAL_LIBRARY_DIRECTORY}"
              COMPONENT MachinekitHAL_Library_RasPi_CPUInfo_Libraries
              NAMELINK_COMPONENT MachinekitHAL_Library_RasPi_CPUInfo_Namelinks
      PUBLIC_HEADER
        DESTINATION "${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/raspi_cpuinfo"
        COMPONENT MachinekitHAL_Library_RasPi_CPUInfo_Headers)

    cpack_add_component(MachinekitHAL_Library_RasPi_CPUInfo_Libraries
                        GROUP MachinekitHAL_Library_RasPi_CPUInfo_Development)
    cpack_add_component(MachinekitHAL_Library_RasPi_CPUInfo_Namelinks
                        GROUP MachinekitHAL_Library_RasPi_CPUInfo_Development)
    cpack_add_component(MachinekitHAL_Library_RasPi_CPUInfo_Headers
                        GROUP MachinekitHAL_Library_RasPi_CPUInfo_Development)

    # Specification of artifacts placement in package tree
    cpack_add_component_group(
      MachinekitHAL_Library_RasPi_CPUInfo_Development
      PARENT_GROUP MachinekitHAL_Package_Base_Libraries_Development)
  endif()
endif()
