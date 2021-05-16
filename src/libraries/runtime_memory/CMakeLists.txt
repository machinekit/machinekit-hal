# ##############################################################################
# Description:  CMakeLists.txt
#
# This file, 'CMakeLists.txt', implements build system rules for Machinekit-HAL
# project
#
# Copyright (C) 2021    Jakub Fišer  <jakub DOT fiser AT eryaf DOT com>
#
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU Lesser General Public License as published by the Free
# Software Foundation; either version 2.1 of the License, or (at your option)
# any later version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
# details.
#
# You should have received a copy of the GNU Lesser General Public License along
# with this library; if not, write to the Free Software Foundation, Inc., 51
# Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
#
# ##############################################################################

option(BUILD_RUNTIME_MEMORY_LIBRARY "Built the Runtime Memory library." TRUE)

if(BUILD_RUNTIME_MEMORY_LIBRARY)
  add_library(runtime_memory_api SHARED)
  add_library(${MACHINEKIT_HAL_NAMESPACE}::runtime_memory_api ALIAS
              runtime_memory_api)

  set(HEADER_FILES include/shmdrv.h)
  set(SOURCE_FILES src/shmdrvapi.c)

  target_include_directories(
    runtime_memory_api
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
           $<INSTALL_INTERFACE:"${MACHINEKIT_HAL_INTERFACE_DIRECTORY}")

  target_sources(runtime_memory_api PRIVATE ${SOURCE_FILES})

  find_library(LIBRT "rt" REQUIRED)

  target_link_libraries(runtime_memory_api PRIVATE runtime_api ${LIBRT})

  target_compile_definitions(runtime_memory_api PRIVATE "ULAPI")

  set_target_properties(runtime_memory_api PROPERTIES PUBLIC_HEADER
                                                      ${HEADER_FILES})

  set_target_properties(runtime_memory_api
                        PROPERTIES SOVERSION 0 POSITION_INDEPENDENT_CODE TRUE)
endif()
