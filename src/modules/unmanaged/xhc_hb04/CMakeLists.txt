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

option(BUILD_XHC_HB04_UNMANAGED_MODULE "Build the XHC HB04 unmanaged module."
       TRUE)

if(BUILD_XHC_HB04_UNMANAGED_MODULE)
  add_executable(xhc_hb04)

  set(SOURCE_FILES src/xhc_hb04.cc)

  target_sources(xhc_hb04 PRIVATE ${SOURCE_FILES})

  target_include_directories(
    xhc_hb04 PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

  pkg_check_modules(LIBUSB "libusb-1.0" REQUIRED IMPORTED_TARGET)

  target_link_libraries(xhc_hb04 PRIVATE hal mkini runtime_math
                                         PkgConfig::LIBUSB)

  target_compile_definitions(xhc_hb04 PRIVATE "PACKAGE_VERSION=00000000")
endif()
