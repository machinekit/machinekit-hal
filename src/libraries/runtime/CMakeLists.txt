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

add_library(runtime_api INTERFACE)
add_library(${MACHINEKIT_HAL_NAMESPACE}::runtime_api ALIAS runtime_api)
add_library(runtime SHARED)
add_library(${MACHINEKIT_HAL_NAMESPACE}::runtime ALIAS runtime)
add_library(runtime_module MODULE)
add_library(${MACHINEKIT_HAL_NAMESPACE}::runtime_module ALIAS runtime_module)

set(SOURCE_FILES 
    src/rtapi.c
    src/rtapi_exception.c
    src/rtapi_flavor.c
    src/rtapi_flavor_hook_wrappers.c
    src/rtapi_heap.c
    src/rtapi_hexdump.c
    src/rtapi_shmem.c
    src/rtapi_task.c
    src/rtapi_time.c)

set(ULAPI_FLAVOR_SOURCE_FILE src/ulapi.c)
set(RT_PREEMPT_FLAVOR_SOURCE_FILE src/rt-preempt.c)

set(HEADER_FILES include/multiframe.h)

target_include_directories(runtime_api INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:"${MACHINEKIT_HAL_INTERFACE_DIRECTORY}"
    $<TARGET_PROPERTY:rtapi_compat,INTERFACE_INCLUDE_DIRECTORIES>)

target_sources(runtime PRIVATE
    $<TARGET_PROPERTY:rtapi_support,SOURCES>
    $<TARGET_PROPERTY:rtapi_compat,SOURCES>
    ${SOURCE_FILES}
    ${ULAPI_FLAVOR_SOURCE_FILE})

target_sources(runtime_module PRIVATE
    ${SOURCE_FILES}
    ${RT_PREEMPT_FLAVOR_SOURCE_FILE})

# TODO if PCI
target_include_directories(runtime_api INTERFACE $<TARGET_PROPERTY:rtapi_pci,INTERFACE_INCLUDE_DIRECTORIES>)
target_sources(runtime PRIVATE $<TARGET_PROPERTY:rtapi_pci,SOURCES>)
target_link_libraries(runtime_api INTERFACE user_pci)
# endif

target_link_libraries(runtime PUBLIC runtime_api runtime_memory_api PRIVATE ${CMAKE_DL_LIBS})
target_link_libraries(runtime_module PRIVATE runtime_api runtime_memory_api syslog_async mkini)

# Jesus Christ, the rtapi headers need hal headers: hal_types.h
target_link_libraries(runtime PRIVATE hal_api)

target_compile_definitions(runtime PUBLIC "ULAPI" "GIT_VERSION=0000000")
target_compile_definitions(runtime_module PRIVATE "RTAPI")

set_target_properties(
  runtime_api PROPERTIES PUBLIC_HEADER ${HEADER_FILES})

set_target_properties(
    runtime
    PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${MACHINEKIT_HAL_LIBRARY_DIRECTORY}
    POSITION_INDEPENDENT_CODE TRUE)

set_target_properties(
    runtime_module
    PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${MACHINEKIT_HAL_MODULE_DIRECTORY})