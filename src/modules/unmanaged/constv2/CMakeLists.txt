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

option(BUILD_CONSTV2_UNMANAGED_MODULE "Build the const/v2 unmanaged module."
       TRUE)

if(BUILD_CONSTV2_UNMANAGED_MODULE)
  add_executable(const)
  add_executable(constv2)

  set(CONST_SOURCE_FILES src/const.cc)
  set(CONSTV2_SOURCE_FILES src/constv2.cc)

  target_sources(const PRIVATE ${CONST_SOURCE_FILES})
  target_sources(constv2 PRIVATE ${CONSTV2_SOURCE_FILES})

  target_include_directories(
    const
    PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/constv2>)
  target_include_directories(
    constv2
    PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/constv2>)

  find_library(LIBMATH NAMES "m" REQUIRED)

  target_link_libraries(const PRIVATE runtime hal mkini ${LIBMATH})
  target_link_libraries(constv2 PRIVATE runtime hal mkini ${LIBMATH})
endif()
