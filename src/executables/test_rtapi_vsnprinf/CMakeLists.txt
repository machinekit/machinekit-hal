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

# TODO: Look into why this causes compilation error on different architectures
if(CMAKE_SYSTEM_PROCESSOR MATCHES "^x86.*")
  option(BUILD_TEST_RTAPI_VSNPRINTF_EXECUTABLE
         "Built the Test RTAPI vsnprintf executable." TRUE)

  if(BUILD_TEST_RTAPI_VSNPRINTF_EXECUTABLE)
    add_executable(test_rtapi_vsnprintf)

    set(SOURCE_FILES src/test_rtapi_vsnprintf.c)
    set(PRIVATE_HEADER_FILES src/vsnprintf.h)

    target_sources(test_rtapi_vsnprintf PRIVATE ${SOURCE_FILES}
                                                ${PRIVATE_HEADER_FILES})

    target_include_directories(
      test_rtapi_vsnprintf
      PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

    target_link_libraries(test_rtapi_vsnprintf PRIVATE runtime runtime_math)
  endif()
endif()
