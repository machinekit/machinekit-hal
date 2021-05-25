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

cmake_minimum_required(VERSION 3.20)

project(
  Machinekit-HAL
  VERSION 0.4 # TODO: Get the right version from VERSION file and git
  DESCRIPTION "Universal framework for machine control."
  HOMEPAGE_URL "https://machinekit.io"
  LANGUAGES C CXX ASM)

if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_SOURCE_DIR}")
  # include(cmake/packaging/packaging.cmake)
  include(CTest)
  enable_testing()
endif()

string(REGEX REPLACE "[-~ .]" "_" MACHINEKIT_HAL_PACKAGE_PREFIX_NORMALIZED
                     ${CMAKE_PROJECT_NAME})
string(TOLOWER ${MACHINEKIT_HAL_PACKAGE_PREFIX_NORMALIZED}
               MACHINEKIT_HAL_PACKAGE_PREFIX)
string(REGEX REPLACE "_" "/" MACHINEKIT_HAL_PACKAGE_PREFIX_PATH
                     ${MACHINEKIT_HAL_PACKAGE_PREFIX})

include(GNUInstallDirs)

set(MACHINEKIT_HAL_NAMESPACE "Machinekit::HAL")
set(MACHINEKIT_HAL_EXECUTABLE_DIRECTORY "${CMAKE_INSTALL_BINDIR}")
set(MACHINEKIT_HAL_LIBRARY_DIRECTORY "${CMAKE_INSTALL_LIBDIR}")
set(MACHINEKIT_HAL_MANAGED_MODULE_DIRECTORY
    "${CMAKE_INSTALL_LIBDIR}/${MACHINEKIT_HAL_PACKAGE_PREFIX_PATH}/module/managed"
)
set(MACHINEKIT_HAL_INTERNAL_EXECUTABLE_DIRECTORY
    "${CMAKE_INSTALL_LIBEXECDIR}/${MACHINEKIT_HAL_PACKAGE_PREFIX_PATH}")
set(MACHINEKIT_HAL_INTERFACE_DIRECTORY "${CMAKE_INSTALL_INCLUDEDIR}")
set(MACHINEKIT_HAL_SYSTEM_CONFIG_DIRECTORY
    "${CMAKE_INSTALL_SYSCONFDIR}/${MACHINEKIT_HAL_PACKAGE_PREFIX_PATH}")
set(MACHINEKIT_HAL_LOCAL_STATE_DIRECTORY "${CMAKE_INSTALL_LOCALSTATEDIR}")

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/tools")

set(CMAKE_C_STANDARD 11)
set(CMAKE_CC_STANDARD 17)

# TODO: Make real effort at setting relevant flags
set(CMAKE_C_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_C_FLAGS_RELEASE "-O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O0")
set(CMAKE_C_FLAGS_RELWITHDEBINFO "-g -O0")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-g -O0")

set(MACHINEKIT_HAL_GIT_BUILD_SHA "aaabbbccc") # TODO: Set based on actual Git
                                              # SHA

add_subdirectory(src)
