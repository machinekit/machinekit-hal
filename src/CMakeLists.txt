#####################################################################
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
######################################################################

include(FindPkgConfig)
include(CheckIncludeFiles)

set(THREADS_PREFER_PTHREAD_FLAG TRUE)

option(BUILD_PYTHON_DISTRIBUTIONS "Build Python3 distribution packages" TRUE)

if(BUILD_PYTHON_DISTRIBUTIONS)
    include(PEP503PythonIndex)
    # Create a simple PEP503 conforming local Python index
    set(PYTHON_INDEX ${CMAKE_BINARY_DIR}/python_index)

    string(REGEX REPLACE "[-~ .]" "_" PYTHON_PACKAGE_PREFIX_NORMALIZED ${CMAKE_PROJECT_NAME})
    string(TOLOWER ${PYTHON_PACKAGE_PREFIX_NORMALIZED} PYTHON_PACKAGE_PREFIX)
    string(REGEX REPLACE "_" "/" PYTHON_PACKAGE_PREFIX_PATH ${PYTHON_PACKAGE_PREFIX})
endif()

add_subdirectory(libraries)
add_subdirectory(executables)
add_subdirectory(modules)

if(BUILD_PYTHON_DISTRIBUTIONS)
    
endif()
