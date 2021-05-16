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

option(BUILD_MB2HAL_UNMANAGED_MODULE "Build the MB2HAL unmanaged module." TRUE)

if(BUILD_MB2HAL_UNMANAGED_MODULE)
  find_package(Threads REQUIRED)
  add_executable(mb2hal)

  set(SOURCE_FILES src/mb2hal.c src/mb2hal_hal.c src/mb2hal_init.c
                   src/mb2hal_modbus.c)

  target_sources(mb2hal PRIVATE ${SOURCE_FILES})

  target_include_directories(
    mb2hal
    PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/mb2hal>)

  pkg_check_modules(LIBMODBUS "libmodbus" REQUIRED IMPORTED_TARGET)

  target_link_libraries(mb2hal PRIVATE hal mkini PkgConfig::LIBMODBUS
                                       Threads::Threads)
endif()
