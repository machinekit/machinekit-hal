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

option(BUILD_HALSCOPE_UNMANAGED_MODULE "Build the HALscope unmanaged module."
       TRUE)

if(BUILD_HALSCOPE_UNMANAGED_MODULE)
  add_executable(halscope)

  set(SOURCE_FILES src/scope.c src/scope_disp.c src/scope_files.c
                   src/scope_horiz.c src/scope_trig.c src/scope_vert.c)

  target_sources(halscope PRIVATE ${SOURCE_FILES})

  target_include_directories(
    halscope
    PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/halscope>)

  target_link_libraries(halscope PRIVATE halscope_memory runtime hal miscgtk
                                         runtime_math)

  target_compile_definitions(
    halscope PRIVATE "HAL_PO_DIR=\"${CMAKE_CURRENT_BINARY_FIR}/share/locale\""
                     "HAL_BIN_DIR=\"${CMAKE_CURRENT_BINARY_FIR}/bin\"")
endif()
