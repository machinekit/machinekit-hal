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

option(BUILD_HOSTMOT2_CORE_MANAGED_DRIVER_MODULE
       "Built the Hostmot2 Core HAL managed driver module." TRUE)

if(BUILD_HOSTMOT2_CORE_MANAGED_DRIVER_MODULE)
  include(MachinekitHALSymbolVisibility)

  add_library(hostmot2_api INTERFACE)
  add_library(${MACHINEKIT_HAL_NAMESPACE}::hostmot2_api ALIAS hostmot2_api)

  add_library(hostmot2_core MODULE)
  add_library(${MACHINEKIT_HAL_NAMESPACE}::hostmot2_core ALIAS hostmot2_core)

  set(SOURCE_FILES
      ${CMAKE_CURRENT_SOURCE_DIR}/src/abs_encoder.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/bspi.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/capsense.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/dbspi.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/dpll.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/encoder.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/fwid.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/hostmot2.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/ioport.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/irq.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/led.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/nano_soc_adc.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/pins.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/pktuart.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/pwmgen.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/raw.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/resolver.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/sserial.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/stepgen.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/tp_pwmgen.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/tram.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/uart.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/watchdog.c)

  set(PUBLIC_HEADER_FILES
      ${CMAKE_CURRENT_SOURCE_DIR}/include/hostmot2/hostmot2-lowlevel.h
      ${CMAKE_CURRENT_SOURCE_DIR}/include/hostmot2/hostmot2.h
      ${CMAKE_CURRENT_SOURCE_DIR}/include/hostmot2/sserial.h)

  target_sources(hostmot2_core PRIVATE ${SOURCE_FILES})

  target_sources(hostmot2_api INTERFACE ${PUBLIC_HEADER_FILES})

  target_link_libraries(
    hostmot2_core
    PRIVATE runtime_api
            hal_api
            hostmot2_api
            hostmot2_bitfile
            user_pci
            runtime_math
            machinetalk_proto_c
            nanopb_interface)

  target_include_directories(
    hostmot2_api
    INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
              $<INSTALL_INTERFACE:${MACHINEKIT_HAL_INTERFACE_DIRECTORY}>)

  target_compile_definitions(hostmot2_core PRIVATE "RTAPI")

  export_rtapi_symbols(TARGET hostmot2_core)

  set_target_properties(
    hostmot2_core
    PROPERTIES OUTPUT_NAME "hostmot2"
               PREFIX "mod"
               LIBRARY_OUTPUT_DIRECTORY
               ${MACHINEKIT_HAL_MANAGED_MODULE_OUTPUT_DIRECTORY})

  set_target_properties(hostmot2_api PROPERTIES PUBLIC_HEADER
                                                "${PUBLIC_HEADER_FILES}")

  install(
    TARGETS hostmot2_api hostmot2_core
    PUBLIC_HEADER DESTINATION "${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/hostmot2"
                  COMPONENT MachinekitHAL_Managed_Module_Hotstmot2_Headers
    LIBRARY DESTINATION "${MACHINEKIT_HAL_MANAGED_MODULE_DIRECTORY}"
            COMPONENT MachinekitHAL_Managed_Module_Hostmot2_Core_Drivers)

  cpack_add_component(MachinekitHAL_Managed_Module_Hotstmot2_Headers
                      GROUP MachinekitHAL_Managed_Module_Hotstmot2_Development)

  cpack_add_component(
    MachinekitHAL_Managed_Module_Hostmot2_Core_Drivers
    GROUP MachinekitHAL_Managed_Module_Hostmot2_Core
    DEPENDS MachinekitHAL_Library_Runtime_Math_Libraries
            MachinekitHAL_Library_Hostmot_Bitfile_Libraries
            MachinekitHAL_Library_Proto_Libraries)

  # Specification of artifacts placement in package tree
  cpack_add_component_group(
    MachinekitHAL_Managed_Module_Hostmot2_Core
    PARENT_GROUP MachinekitHAL_Package_Base_Managed_Modules_Drivers)
  cpack_add_component_group(
    MachinekitHAL_Managed_Module_Hotstmot2_Development
    PARENT_GROUP MachinekitHAL_Package_Base_Managed_Modules_Drivers_Development)

  add_subdirectory(test)
endif()
