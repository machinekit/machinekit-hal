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

option(BUILD_RTAPI_PCI_LIBRARY "Built the RTAPI PCI libraries." TRUE)

if(BUILD_RTAPI_PCI_LIBRARY)
  add_library(rtapi_pci INTERFACE)

  set(SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/rtapi_pci.c)
  set(PUBLIC_HEADER_FILES
      ${CMAKE_CURRENT_SOURCE_DIR}/include/runtime/rtapi_pci.h)

  target_sources(rtapi_pci INTERFACE ${SOURCE_FILES} ${PUBLIC_HEADER_FILES})

  target_include_directories(
    rtapi_pci
    INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
              $<INSTALL_INTERFACE:${MACHINEKIT_HAL_INTERFACE_DIRECTORY}>)

  set_target_properties(rtapi_pci PROPERTIES PUBLIC_HEADER
                                             "${PUBLIC_HEADER_FILES}")

  install(
    TARGETS rtapi_pci
    EXPORT machinekit_hal_rtapi_pci
    PUBLIC_HEADER DESTINATION "${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/runtime"
                  COMPONENT MachinekitHAL_Library_RTAPI_PCI_Headers)

  cpack_add_component(MachinekitHAL_Library_RTAPI_PCI_Headers
                      GROUP MachinekitHAL_Library_RTAPI_PCI_Development)

  # Specification of artifacts placement in package tree
  cpack_add_component_group(
    MachinekitHAL_Library_RTAPI_PCI_Development
    PARENT_GROUP MachinekitHAL_Package_Base_Libraries_Development)
endif()
