# ~~~
# ####################################################################
# Description:  debianToolchain.cmake
#
#               This file, 'debianToolchain.cmake', implements an universal
#               CMake toolchain file for building Machinekit-HAL suite for
#               all supported architectures on Debian-based operating systems.
#
#               This process presumes that the environment variables include
#               all items from 'dpkg-architecture' output. This means it is either
#               running as part of the 'dpkg-buildpackage' workflow, or the
#               process just have the variables manually exported to 'environ'.
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
# ####################################################################
# ~~~

if(DEFINED ENV{DEB_BUILD_GNU_TYPE}
   AND DEFINED ENV{DEB_HOST_GNU_TYPE}
   AND DEFINED ENV{DEB_HOST_ARCH_CPU}
   AND DEFINED ENV{DEB_HOST_ARCH_OS})

  if(NOT "$ENV{DEB_BUILD_GNU_TYPE}" STREQUAL "$ENV{DEB_HOST_GNU_TYPE}")
    # CMake known value required, i.e. 'Linux' with capital 'L'
    set(CMAKE_SYSTEM_NAME "Linux")
    set(CMAKE_SYSTEM_PROCESSOR "$ENV{DEB_HOST_ARCH_CPU}")
    set(CMAKE_LIBRARY_ARCHITECTURE "$ENV{DEB_HOST_GNU_TYPE}")
  endif()

  # Try to find the HOST GCC compiler in the ${PATH}, fail quickly if the tool
  # cannot be found
  find_program(_toolchain_gcc_compiler "$ENV{DEB_HOST_GNU_TYPE}-gcc" REQUIRED)
  # Try to find the HOST G++ compiler in the ${PATH}, fail quickly if the tool
  # cannot be found
  find_program(_toolchain_gplusplus_compiler "$ENV{DEB_HOST_GNU_TYPE}-g++"
               REQUIRED)
  # Try to find the HOST Pkg-Config in the ${PATH}, do not fail if the tool
  # cannot be found, as the BUILD one can be used with setting the right
  # environment variable to the root of '.pc' files in HOST libraries directory
  find_program(_toolchain_dpkg_cross_pkg_config
               "$ENV{DEB_HOST_GNU_TYPE}-pkg-config")

  # Populate the CMake variables with the full paths of the previously found
  # compilers, this works around some specific CMake issues
  set(CMAKE_C_COMPILER "${_toolchain_gcc_compiler}")
  set(CMAKE_CXX_COMPILER "${_toolchain_gplusplus_compiler}")

  # This is true only and always the CMAKE_SYSTEM_NAME variable was set either
  # in this toolchain file or on the command line when the CMake
  # configuration/generation is requested
  if(CMAKE_CROSSCOMPILING)
    set(ONLY_CMAKE_FIND_ROOT_PATH TRUE)
    set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
    set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
    set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
    set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

    # Prepend the architecture specific include directories to the compiler
    # search path
    include_directories(BEFORE "/usr/$ENV{DEB_HOST_GNU_TYPE}/include"
                        "/usr/include/$ENV{DEB_HOST_GNU_TYPE}")

    # Use the HOST architecture specific DPKG-Config and fall back if one doe
    # not exist
    if(EXISTS "${_toolchain_dpkg_cross_pkg_config}")
      set(PKG_CONFIG_EXECUTABLE "${_toolchain_dpkg_cross_pkg_config}")
    else()
      set(ENV{PKG_CONFIG_LIBDIR}
          "/usr/lib/$ENV{DEB_HOST_GNU_TYPE}/pkgconfig:/usr/share/pkgconfig")
    endif()
  endif()
endif()
