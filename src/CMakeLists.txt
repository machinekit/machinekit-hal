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
    if(NOT MACHINEKIT_HAL_PYTHON_INDEX)
        set(MACHINEKIT_HAL_PYTHON_INDEX "${CMAKE_BINARY_DIR}/$<CONFIG>/python_index")
    endif()
endif()

# Define the outer rules for output directories for build artifacts
if(NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>/${MACHINEKIT_HAL_EXECUTABLE_DIRECTORY}")
endif()
if(NOT CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>/${MACHINEKIT_HAL_LIBRARY_DIRECTORY}")
endif()
if(NOT CMAKE_LIBRARY_OUTPUT_DIRECTORY)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>/${MACHINEKIT_HAL_LIBRARY_DIRECTORY}")
endif()
# Extensions to standard CMake-defined OUTPUT directories for Machinekit-HAL
if(NOT MACHINEKIT_HAL_MANAGED_MODULE_OUTPUT_DIRECTORY)
    set(MACHINEKIT_HAL_MANAGED_MODULE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>/${MACHINEKIT_HAL_MODULE_DIRECTORY}")
endif()
if(NOT MACHINEKIT_HAL_CONFIG_OUTPUT_DIRECTORY)
    set(MACHINEKIT_HAL_CONFIG_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>/${MACHINEKIT_HAL_SYSTEM_CONFIG_DIRECTORY}")
endif()
if(NOT MACHINEKIT_HAL_INTERNAL_EXECUTABLE_OUTPUT_DIRECTORY)
    set(MACHINEKIT_HAL_INTERNAL_EXECUTABLE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>/${MACHINEKIT_HAL_INTERNAL_EXECUTABLE_DIRECTORY}")
endif()

add_subdirectory(libraries)
add_subdirectory(executables)
add_subdirectory(modules)

if(BUILD_PYTHON_DISTRIBUTIONS)
    
endif()
