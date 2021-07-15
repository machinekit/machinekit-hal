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

include(MachinekitHALSymbolVisibility)

add_library(hostmot2_test MODULE)

set(SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/hostmot2_test/src/hm2_test.c)

set(PRIVATE_HEADER_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/hostmot2_test/src/hm2_test.h)

target_sources(hostmot2_test PRIVATE ${SOURCE_FILES} ${PRIVATE_HEADER_FILES})

target_link_libraries(hostmot2_test PRIVATE runtime_api hal_api hostmot2_api
                                            hostmot2_bitfile)

target_compile_definitions(hostmot2_test PRIVATE "RTAPI")

export_rtapi_symbols(TARGET hostmot2_test)

set_target_properties(
  hostmot2_test
  PROPERTIES OUTPUT_NAME "hm2_test"
             PREFIX "mod"
             LIBRARY_OUTPUT_DIRECTORY
             "${MACHINEKIT_HAL_TEST_OUTPUT_DIRECTORY}/runtests/hm2-idrom")

install(
  TARGETS hostmot2_test
  LIBRARY DESTINATION "${MACHINEKIT_HAL_TEST_DIRECTORY}/runtests/hm2-idrom"
          COMPONENT MachinekitHAL_Managed_Module_Hostmot2_Test_Drivers)

cpack_add_component(
  MachinekitHAL_Managed_Module_Hostmot2_Test_Drivers
  GROUP MachinekitHAL_Managed_Module_Hostmot2_Test
  DEPENDS MachinekitHAL_Managed_Module_Hostmot2_Core_Drivers)

# Specification of artifacts placement in package tree
cpack_add_component_group(
  MachinekitHAL_Managed_Module_Hostmot2_Test
  PARENT_GROUP MachinekitHAL_Package_Base_Tests_Runtests)
