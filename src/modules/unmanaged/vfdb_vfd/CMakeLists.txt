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

option(BUILD_VFDB_VFD_UNMANAGED_MODULE "Build the VFDB VFD unmanaged module."
       TRUE)

if(BUILD_VFDB_VFD_UNMANAGED_MODULE)
  add_executable(vfdb_vfd)

  set(SOURCE_FILES src/vfdb_vfd.c)

  target_sources(vfdb_vfd PRIVATE ${SOURCE_FILES})

  pkg_check_modules(LIBMODBUS "libmodbus" REQUIRED IMPORTED_TARGET)
  find_library(LIBMATH NAMES "m" REQUIRED)

  target_link_libraries(
    vfdb_vfd
    runtime
    hal
    mkini
    runtime_math
    PkgConfig::LIBMODBUS
    ${LIBMATH})
endif()
