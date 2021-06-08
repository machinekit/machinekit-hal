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

if(NOT DEFINED SYS_IO_AVAILABLE)
  check_include_files("sys/io.h" SYS_IO_AVAILABLE)
endif()

if(SYS_IO_AVAILABLE)
  option(BUILD_RTAPI_PCI_LIBRARY "Built the RTAPI PCI libraries." TRUE)

  if(BUILD_RTAPI_PCI_LIBRARY)
    add_library(rtapi_pci INTERFACE)

    target_sources(rtapi_pci INTERFACE src/rtapi_pci.c)

    target_include_directories(
      rtapi_pci
      INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                $<INSTALL_INTERFACE:${MACHINEKIT_HAL_INTERFACE_DIRECTORY}>)

    set_target_properties(rtapi_pci PROPERTIES PUBLIC_HEADER
                                               include/rtapi_pci.h)

    install(
      TARGETS rtapi_pci
      EXPORT machinekit_hal
      PUBLIC_HEADER DESTINATION "${MACHINEKIT_HAL_INTERFACE_DIRECTORY}"
                    COMPONENT MachinekitHAL_Library_Development)
  endif()
endif()
