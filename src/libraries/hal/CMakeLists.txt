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

option(BUILD_HAL_LIBRARY "Built the HAL library." TRUE)

if(BUILD_HAL_LIBRARY)
  include(MachinekitHALSymbolVisibility)

  add_library(hal_api INTERFACE)
  add_library(${MACHINEKIT_HAL_NAMESPACE}::hal_api ALIAS hal_api)
  add_library(hal SHARED)
  add_library(${MACHINEKIT_HAL_NAMESPACE}::hal ALIAS hal)
  # Technically not a library, but managed module
  add_library(hal_module MODULE)
  add_library(${MACHINEKIT_HAL_NAMESPACE}::hal_module ALIAS hal_module)

  set(SOURCE_FILES
      ${CMAKE_CURRENT_SOURCE_DIR}/src/hal_lib.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/hal_group.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/hal_ring.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/hal_rcomp.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/hal_vtable.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/hal_funct.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/hal_thread.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/hal_param.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/hal_signal.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/hal_pin.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/hal_comp.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/hal_memory.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/hal_misc.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/hal_instance.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/hal_object.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/hal_object_selectors.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/hal_accessor.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/hal_iring.c)

  set(HALPB_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/halpb.cc)

  set(PUBLIC_HEADER_FILES
      ${CMAKE_CURRENT_SOURCE_DIR}/include/hal/config_module.h
      ${CMAKE_CURRENT_SOURCE_DIR}/include/hal/hal_accessor.h
      ${CMAKE_CURRENT_SOURCE_DIR}/include/hal/hal_accessor_macros.h
      ${CMAKE_CURRENT_SOURCE_DIR}/include/hal/hal_group.h
      ${CMAKE_CURRENT_SOURCE_DIR}/include/hal/hal.h
      ${CMAKE_CURRENT_SOURCE_DIR}/include/hal/hal_internal.h
      ${CMAKE_CURRENT_SOURCE_DIR}/include/hal/hal_iring.h
      ${CMAKE_CURRENT_SOURCE_DIR}/include/hal/hal_iter.h
      ${CMAKE_CURRENT_SOURCE_DIR}/include/hal/hal_list.h
      ${CMAKE_CURRENT_SOURCE_DIR}/include/hal/hal_logging.h
      ${CMAKE_CURRENT_SOURCE_DIR}/include/hal/hal_object.h
      ${CMAKE_CURRENT_SOURCE_DIR}/include/hal/hal_object_selectors.h
      ${CMAKE_CURRENT_SOURCE_DIR}/include/hal/halpb.hh
      ${CMAKE_CURRENT_SOURCE_DIR}/include/hal/hal_priv.h
      ${CMAKE_CURRENT_SOURCE_DIR}/include/hal/hal_rcomp.h
      ${CMAKE_CURRENT_SOURCE_DIR}/include/hal/hal_ring.h
      ${CMAKE_CURRENT_SOURCE_DIR}/include/hal/hal_types.h
      ${CMAKE_CURRENT_SOURCE_DIR}/include/hal/vtable.h)
  # TODO: Filter out header files from HAL which can safely be private
  # set(PRIVATE_HEADER_FILES "")

  target_include_directories(
    hal_api
    INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
              $<INSTALL_INTERFACE:"${MACHINEKIT_HAL_INTERFACE_DIRECTORY}")

  target_sources(hal PRIVATE ${SOURCE_FILES} ${HALPB_SOURCE_FILES})
  target_sources(hal_module PRIVATE ${SOURCE_FILES})

  target_link_libraries(
    hal
    PUBLIC hal_api runtime_api runtime
    PRIVATE mkini machinetalk_proto_cc)
  target_link_libraries(hal_module PRIVATE hal_api runtime_api)

  target_compile_definitions(hal_module PRIVATE "RTAPI")

  export_rtapi_symbols(TARGET hal_module)

  set_target_properties(hal_api PROPERTIES PUBLIC_HEADER "${PUBLIC_HEADER_FILES}")

  set_target_properties(
    hal
    PROPERTIES SOVERSION 0
               VERSION ${CMAKE_PROJECT_VERSION}
               POSITION_INDEPENDENT_CODE TRUE)

  set_target_properties(
    hal_module
    PROPERTIES OUTPUT_NAME "hal"
               PREFIX "mod"
               LIBRARY_OUTPUT_DIRECTORY
               ${MACHINEKIT_HAL_MANAGED_MODULE_OUTPUT_DIRECTORY})

  install(
    TARGETS hal hal_api
    EXPORT machinekit_hal
    LIBRARY DESTINATION "${MACHINEKIT_HAL_LIBRARY_DIRECTORY}"
            COMPONENT MachinekitHAL_Library_Base
            NAMELINK_COMPONENT MachinekitHAL_Library_Development
    PUBLIC_HEADER DESTINATION "${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/hal"
                  COMPONENT MachinekitHAL_Library_Development)

  install(
    TARGETS hal_module
    EXPORT machinekit_hal_managed_module
    LIBRARY DESTINATION "${MACHINEKIT_HAL_MANAGED_MODULE_DIRECTORY}"
            COMPONENT MachinekitHAL_Managed_Module_Base)
endif()
