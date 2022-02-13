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

include(FindPkgConfig)
include(CheckIncludeFiles)

set(THREADS_PREFER_PTHREAD_FLAG TRUE)

option(BUILD_PYTHON_DISTRIBUTIONS "Build Python3 distribution packages" TRUE)

# TODO: Allow sitting this from option and/or automatically check this exists
set(BASH_SHEBANG "#!/usr/bin/env bash")

# Define the outer rules for output directories for build artifacts
set(MACHINEKIT_HAL_ARTIFACTS_MOUNTPOINT_DIRECTORY
    "${CMAKE_BINARY_DIR}/$<CONFIG>")
if(NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY
      "${MACHINEKIT_HAL_ARTIFACTS_MOUNTPOINT_DIRECTORY}/${MACHINEKIT_HAL_EXECUTABLE_DIRECTORY}"
  )
endif()
if(NOT CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY
      "${MACHINEKIT_HAL_ARTIFACTS_MOUNTPOINT_DIRECTORY}/${MACHINEKIT_HAL_LIBRARY_DIRECTORY}"
  )
endif()
if(NOT CMAKE_LIBRARY_OUTPUT_DIRECTORY)
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY
      "${MACHINEKIT_HAL_ARTIFACTS_MOUNTPOINT_DIRECTORY}/${MACHINEKIT_HAL_LIBRARY_DIRECTORY}"
  )
endif()
# Extensions to standard CMake-defined OUTPUT directories for Machinekit-HAL
if(NOT MACHINEKIT_HAL_MANAGED_MODULE_OUTPUT_DIRECTORY)
  set(MACHINEKIT_HAL_MANAGED_MODULE_OUTPUT_DIRECTORY
      "${MACHINEKIT_HAL_ARTIFACTS_MOUNTPOINT_DIRECTORY}/${MACHINEKIT_HAL_MANAGED_MODULE_DIRECTORY}"
  )
endif()
if(NOT MACHINEKIT_HAL_UNMANAGED_MODULE_OUTPUT_DIRECTORY)
  set(MACHINEKIT_HAL_UNMANAGED_MODULE_OUTPUT_DIRECTORY
      "${MACHINEKIT_HAL_ARTIFACTS_MOUNTPOINT_DIRECTORY}/${MACHINEKIT_HAL_UNMANAGED_MODULE_DIRECTORY}"
  )
endif()
if(NOT MACHINEKIT_HAL_SYSTEM_CONFIG_OUTPUT_DIRECTORY)
  set(MACHINEKIT_HAL_SYSTEM_CONFIG_OUTPUT_DIRECTORY
      "${MACHINEKIT_HAL_ARTIFACTS_MOUNTPOINT_DIRECTORY}/${MACHINEKIT_HAL_SYSTEM_CONFIG_DIRECTORY}"
  )
endif()
if(NOT MACHINEKIT_HAL_INTERNAL_EXECUTABLE_OUTPUT_DIRECTORY)
  set(MACHINEKIT_HAL_INTERNAL_EXECUTABLE_OUTPUT_DIRECTORY
      "${MACHINEKIT_HAL_ARTIFACTS_MOUNTPOINT_DIRECTORY}/${MACHINEKIT_HAL_INTERNAL_EXECUTABLE_DIRECTORY}"
  )
endif()
if(NOT MACHINEKIT_HAL_LOCAL_STATE_OUTPUT_DIRECTORY)
  set(MACHINEKIT_HAL_LOCAL_STATE_OUTPUT_DIRECTORY
      "${MACHINEKIT_HAL_ARTIFACTS_MOUNTPOINT_DIRECTORY}/${MACHINEKIT_HAL_LOCAL_STATE_DIRECTORY}"
  )
endif()
if(NOT MACHINEKIT_HAL_TEST_OUTPUT_DIRECTORY)
  set(MACHINEKIT_HAL_TEST_OUTPUT_DIRECTORY
      "${MACHINEKIT_HAL_ARTIFACTS_MOUNTPOINT_DIRECTORY}/${MACHINEKIT_HAL_TEST_DIRECTORY}"
  )
endif()
# The export contains versions for all configuration, so there is no need for
# the configuration specific ${MACHINEKIT_HAL_ARTIFACTS_MOUNTPOINT_DIRECTORY}
# (and it even cannot be used for generator expression in it), but to have
# direct similarity to install TREE, use the './lib/<GNU-triplet>/cmake' format
# even for BUILD tree (even though it may seem odd) TODO: Investigate if this is
# really the best solution (as the relative path change does not matter much)
if(NOT MACHINEKIT_HAL_CMAKE_PACKAGE_OUTPUT_DIRECTORY)
  set(MACHINEKIT_HAL_CMAKE_PACKAGE_OUTPUT_DIRECTORY
      "${CMAKE_BINARY_DIR}/${MACHINEKIT_HAL_CMAKE_PACKAGE_DIRECTORY}")
endif()

# Full install paths for Machinekit-HAL specific directories
GNUInstallDirs_get_absolute_install_dir(
  MACHINEKIT_HAL_EXECUTABLE_FULL_INSTALL_DIRECTORY
  MACHINEKIT_HAL_EXECUTABLE_DIRECTORY "BINDIR")
GNUInstallDirs_get_absolute_install_dir(
  MACHINEKIT_HAL_LIBRARY_FULL_INSTALL_DIRECTORY
  MACHINEKIT_HAL_LIBRARY_DIRECTORY "LIBDIR")
GNUInstallDirs_get_absolute_install_dir(
  MACHINEKIT_HAL_MANAGED_FULL_INSTALL_MODULE_DIRECTORY
  MACHINEKIT_HAL_MANAGED_MODULE_DIRECTORY "LIBDIR")
GNUInstallDirs_get_absolute_install_dir(
  MACHINEKIT_HAL_UNMANAGED_FULL_INSTALL_MODULE_DIRECTORY
  MACHINEKIT_HAL_UNMANAGED_MODULE_DIRECTORY "BINDIR")
GNUInstallDirs_get_absolute_install_dir(
  MACHINEKIT_HAL_INTERNAL_EXECUTABLE_FULL_INSTALL_DIRECTORY
  MACHINEKIT_HAL_INTERNAL_EXECUTABLE_DIRECTORY "LIBEXECDIR")
GNUInstallDirs_get_absolute_install_dir(
  MACHINEKIT_HAL_INTERFACE_FULL_INSTALL_DIRECTORY
  MACHINEKIT_HAL_INTERFACE_DIRECTORY "INCLUDEDIR")
GNUInstallDirs_get_absolute_install_dir(
  MACHINEKIT_HAL_SYSTEM_CONFIG_FULL_INSTALL_DIRECTORY
  MACHINEKIT_HAL_SYSTEM_CONFIG_DIRECTORY "SYSCONFDIR")
GNUInstallDirs_get_absolute_install_dir(
  MACHINEKIT_HAL_LOCAL_STATE_FULL_INSTALL_DIRECTORY
  MACHINEKIT_HAL_LOCAL_STATE_DIRECTORY "LOCALSTATEDIR")
GNUInstallDirs_get_absolute_install_dir(
  MACHINEKIT_HAL_TEST_FULL_INSTALL_DIRECTORY MACHINEKIT_HAL_TEST_DIRECTORY
  "DATAROOTDIR")
GNUInstallDirs_get_absolute_install_dir(
  MACHINEKIT_HAL_CMAKE_PACKAGE_FULL_INSTALL_DIRECTORY
  MACHINEKIT_HAL_CMAKE_PACKAGE_DIRECTORY "LIBDIR")

if(BUILD_PYTHON_DISTRIBUTIONS)
  #[[
  Installation of Python modules requires a higher logic for determining where
  to put the files on the filesystem. Depending on the exact way how the Machinekit-HAL
  installation will be used (Debian [and other linux distributions's] packaging,
  local, /opt, user's HOME installation schemas or virtual environments), the paths
  will be different and DOES NOT need to be in any way depending on what GNUInstallDirs
  CMake module is defining for C and C++ language targets

  TODO: Write smarter logic for default paths

  This SIMPLE setting of defaults for full installation folders to the current active
  "Python" using the FindPython CMake module

  What FindPython reports is superior to what 'sysconfig.get_paths()' reports, at leas
  on Debian system (some reported paths do NOT exist):

  Python_STDLIB: '/usr/lib/python3.9'
  Python_STDARCH:'/usr/lib/python3.9'
  Python_SITELIB:'/usr/lib/python3/dist-packages'
  Python_SITEARCH:'/usr/lib/python3/dist-packages'
  Python_INCLUDE_DIRS:'/usr/include/python3.9'
  Python_LIBRARY_DIRS:'/usr/lib/x86_64-linux-gnu'
  Python_RUNTIME_LIBRARY_DIRS:'/usr/lib/x86_64-linux-gnu'

  vs

  {'data': '/usr',
  'include': '/usr/include/python3.9',
  'platinclude': '/usr/include/python3.9',
  'platlib': '/usr/lib/python3.9/site-packages',
  'platstdlib': '/usr/lib/python3.9',
  'purelib': '/usr/lib/python3.9/site-packages',
  'scripts': '/usr/bin',
  'stdlib': '/usr/lib/python3.9'}

  or for VENV virtual environment:

  Python_STDLIB: '/usr/lib/python3.9'
  Python_STDARCH: '/usr/lib/python3.9'
  Python_SITELIB: '/tmp/venv/lib/python3.9/site-packages'
  Python_SITEARCH: '/tmp/venv/lib/python3.9/site-packages'
  Python_INCLUDE_DIRS: '/usr/include/python3.9'
  Python_LIBRARY_DIRS: '/usr/lib/x86_64-linux-gnu'
  Python_RUNTIME_LIBRARY_DIRS: '/usr/lib/x86_64-linux-gnu'

  vs

  {'data': '/tmp/venv',
  include': '/usr/include/python3.9',
  platinclude': '/usr/include/python3.9',
  platlib': '/tmp/venv/lib/python3.9/site-packages',
  platstdlib': '/tmp/venv/lib/python3.9',
  purelib': '/tmp/venv/lib/python3.9/site-packages',
  scripts': '/tmp/venv/bin',
  stdlib': '/usr/lib/python3.9'}
  ]]

  find_package(
    Python
    COMPONENTS Interpreter Development
    REQUIRED)

  set(MACHINEKIT_HAL_PYTHON_INTERPRETER_INSTALL_EXECUTABLE
      "${Python_EXECUTABLE}"
      CACHE
        FILEPATH
        "Path to the Python3 interpreter which will be used as main execution interpreter for installed COMPONENTS."
  )

  if(NOT MACHINEKIT_HAL_PYTHON_STDLIB_FULL_INSTALL_DIRECTORY)
    set(MACHINEKIT_HAL_PYTHON_STDLIB_FULL_INSTALL_DIRECTORY "${Python_STDLIB}")
  endif()
  if(NOT MACHINEKIT_HAL_PYTHON_PURELIB_FULL_INSTALL_DIRECTORY)
    set(MACHINEKIT_HAL_PYTHON_PURELIB_FULL_INSTALL_DIRECTORY
        "${Python_SITELIB}")
  endif()
  if(NOT MACHINEKIT_HAL_PYTHON_PLATSTDLIB_FULL_INSTALL_DIRECTORY)
    set(MACHINEKIT_HAL_PYTHON_PLATSTDLIB_FULL_INSTALL_DIRECTORY
        "${Python_STDARCH}")
  endif()
  if(NOT MACHINEKIT_HAL_PYTHON_PLATLIB_FULL_INSTALL_DIRECTORY)
    set(MACHINEKIT_HAL_PYTHON_PLATLIB_FULL_INSTALL_DIRECTORY
        "${Python_SITEARCH}")
  endif()
  if(NOT MACHINEKIT_HAL_PYTHON_INCLUDE_FULL_INSTALL_DIRECTORY)
    set(MACHINEKIT_HAL_PYTHON_INCLUDE_FULL_INSTALL_DIRECTORY
        "${Python_INCLUDE_DIRS}")
  endif()
  if(NOT MACHINEKIT_HAL_PYTHON_PLATINCLUDE_FULL_INSTALL_DIRECTORY)
    set(MACHINEKIT_HAL_PYTHON_PLATINCLUDE_FULL_INSTALL_DIRECTORY
        "${Python_INCLUDE_DIRS}")
  endif()
  if(NOT MACHINEKIT_HAL_PYTHON_SCRIPTS_FULL_INSTALL_DIRECTORY)
    set(MACHINEKIT_HAL_PYTHON_SCRIPTS_FULL_INSTALL_DIRECTORY
        "${MACHINEKIT_HAL_EXECUTABLE_FULL_INSTALL_DIRECTORY}")
  endif()
  if(NOT MACHINEKIT_HAL_PYTHON_DATA_FULL_INSTALL_DIRECTORY)
    set(MACHINEKIT_HAL_PYTHON_DATA_FULL_INSTALL_DIRECTORY
        "${CMAKE_INSTALL_PREFIX}")
  endif()

  # Create a simple PEP503 conforming local Python index
  if(NOT MACHINEKIT_HAL_PYTHON_INDEX)
    set(MACHINEKIT_HAL_PYTHON_INDEX
        "${CMAKE_BINARY_DIR}/$<CONFIG>/python_index")
  endif()
  include(PEP503PythonIndex)
endif()

list(APPEND CMAKE_MODULE_PATH
     "${MACHINEKIT_HAL_CMAKE_PACKAGE_OUTPUT_DIRECTORY}")
list(APPEND CMAKE_PREFIX_PATH
     "${MACHINEKIT_HAL_ARTIFACTS_MOUNTPOINT_DIRECTORY}")

add_subdirectory(libraries)
add_subdirectory(executables)
add_subdirectory(modules)

# Terrible, ugly and hopefully temporary hack
add_subdirectory(${CMAKE_SOURCE_DIR}/tests ${CMAKE_BINARY_DIR}/tests)
