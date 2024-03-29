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

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${MACHINEKIT_HAL_TEST_OUTPUT_DIRECTORY}")

add_executable(test_runtime_module_init)

add_executable(test_runtime_bitops)
# add_executable(test_runtime_rtapi_flavor_RTAPI)
add_executable(test_runtime_rtapi_flavor_ULAPI)

set(BITOPS_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/bitops/src/bitops.c)
set(RTAPI_FLAVOR_SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/rtapi_flavor/src/rtapi_flavor.c)
set(MODULE_INIT_SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/module_init/src/module_init.c)

target_sources(test_runtime_bitops PRIVATE ${BITOPS_SOURCE_FILES})
# target_sources(test_runtime_rtapi_flavor_RTAPI PRIVATE
# ${RTAPI_FLAVOR_SOURCE_FILES})
target_sources(test_runtime_rtapi_flavor_ULAPI
               PRIVATE ${RTAPI_FLAVOR_SOURCE_FILES})
target_sources(test_runtime_module_init PRIVATE ${MODULE_INIT_SOURCE_FILES})

pkg_check_modules(CMOCKA "cmocka" REQUIRED IMPORTED_TARGET)

target_link_libraries(test_runtime_bitops PRIVATE unmanaged_runtime)
# target_link_libraries( test_runtime_rtapi_flavor_RTAPI PRIVATE
# unmanaged_runtime PkgConfig::CMOCKA syslog_async runtime_memory_api mkini)
target_link_libraries(
  test_runtime_rtapi_flavor_ULAPI PRIVATE unmanaged_runtime PkgConfig::CMOCKA
                                          syslog_async runtime_memory_api mkini)
target_link_libraries(test_runtime_module_init PRIVATE unmanaged_runtime
                                                       unmanaged_hal)

set_target_properties(
  test_runtime_module_init
  PROPERTIES RUNTIME_OUTPUT_DIRECTORY
             "${MACHINEKIT_HAL_TEST_OUTPUT_DIRECTORY}/runtests/rtapi.0")

add_test(NAME testRuntimeBitops COMMAND test_runtime_bitops)

install(
  TARGETS test_runtime_module_init
  RUNTIME DESTINATION "${MACHINEKIT_HAL_TEST_DIRECTORY}/runtests/rtapi.0"
          COMPONENT MachinekitHAL_TestSuite_Test_Runtime_Module_Init_Binaries)

cpack_add_component(
  MachinekitHAL_TestSuite_Test_Runtime_Module_Init_Binaries
  GROUP MachinekitHAL_TestSuite_Test_Runtime_Module_Init
  DEPENDS MachinekitHAL_Library_Runtime_Libraries
          MachinekitHAL_Library_HAL_Libraries)

# Specification of artifacts placement in package tree
cpack_add_component_group(
  MachinekitHAL_TestSuite_Test_Runtime_Module_Init
  PARENT_GROUP MachinekitHAL_Package_Base_Tests_Runtests)
