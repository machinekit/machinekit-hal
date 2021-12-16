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

option(BUILD_HAL_LIBRARY "Built the HAL library." TRUE)

if(BUILD_HAL_LIBRARY)
  include(MachinekitHALSymbolVisibilityFunction)

  add_library(hal_api INTERFACE)
  # HAL_api target should be considered as internal TARGET of the Machinekit-HAL
  # source tree and should not be exported outside of it
  # add_library(${MACHINEKIT_HAL_NAMESPACE}::hal_api ALIAS hal_api)
  add_library(managed_hal INTERFACE)
  add_library(${MACHINEKIT_HAL_NAMESPACE}::managed_hal ALIAS managed_hal)
  add_library(unmanaged_hal SHARED)
  add_library(${MACHINEKIT_HAL_NAMESPACE}::unmanaged_hal ALIAS unmanaged_hal)
  # Technically not a library, but a managed module to be loaded into
  # Machinekit-HAL's namespace of the managing process
  add_library(hal_module MODULE)
  # HAL_module target should be considered as internal TARGET of the
  # Machinekit-HAL source tree and should not be exported outside of it
  # add_library(${MACHINEKIT_HAL_NAMESPACE}::hal_module ALIAS hal_module)

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

  set(PUBLIC_INCLUDE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include")

  set(PUBLIC_HEADER_FILES_BUILD
      ${PUBLIC_INCLUDE_DIRECTORY}/hal/hal_accessor.h
      ${PUBLIC_INCLUDE_DIRECTORY}/hal/hal_accessor_macros.h
      ${PUBLIC_INCLUDE_DIRECTORY}/hal/hal_group.h
      ${PUBLIC_INCLUDE_DIRECTORY}/hal/hal.h
      ${PUBLIC_INCLUDE_DIRECTORY}/hal/hal_internal.h
      ${PUBLIC_INCLUDE_DIRECTORY}/hal/hal_iring.h
      ${PUBLIC_INCLUDE_DIRECTORY}/hal/hal_iter.h
      ${PUBLIC_INCLUDE_DIRECTORY}/hal/hal_list.h
      ${PUBLIC_INCLUDE_DIRECTORY}/hal/hal_logging.h
      ${PUBLIC_INCLUDE_DIRECTORY}/hal/hal_object.h
      ${PUBLIC_INCLUDE_DIRECTORY}/hal/hal_object_selectors.h
      ${PUBLIC_INCLUDE_DIRECTORY}/hal/halpb.hh
      ${PUBLIC_INCLUDE_DIRECTORY}/hal/hal_priv.h
      ${PUBLIC_INCLUDE_DIRECTORY}/hal/hal_rcomp.h
      ${PUBLIC_INCLUDE_DIRECTORY}/hal/hal_ring.h
      ${PUBLIC_INCLUDE_DIRECTORY}/hal/hal_types.h
      ${PUBLIC_INCLUDE_DIRECTORY}/hal/vtable.h)
  # TODO: Filter out header files from HAL which can safely be private
  # set(PRIVATE_HEADER_FILES "")

  set(PUBLIC_HEADER_FILES_INSTALL
      ${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/hal/hal_accessor.h
      ${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/hal/hal_accessor_macros.h
      ${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/hal/hal_group.h
      ${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/hal/hal.h
      ${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/hal/hal_internal.h
      ${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/hal/hal_iring.h
      ${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/hal/hal_iter.h
      ${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/hal/hal_list.h
      ${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/hal/hal_logging.h
      ${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/hal/hal_object.h
      ${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/hal/hal_object_selectors.h
      ${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/hal/halpb.hh
      ${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/hal/hal_priv.h
      ${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/hal/hal_rcomp.h
      ${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/hal/hal_ring.h
      ${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/hal/hal_types.h
      ${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/hal/vtable.h)
  # TODO: Filter out header files from HAL which can safely be private
  # set(PRIVATE_HEADER_FILES "")

  target_include_directories(
    hal_api
    INTERFACE $<BUILD_INTERFACE:${PUBLIC_INCLUDE_DIRECTORY}>
              $<INSTALL_INTERFACE:${MACHINEKIT_HAL_INTERFACE_DIRECTORY}>)

  target_sources(unmanaged_hal PRIVATE ${SOURCE_FILES} ${HALPB_SOURCE_FILES})
  target_sources(hal_module PRIVATE ${SOURCE_FILES})

  target_link_libraries(
    unmanaged_hal
    PUBLIC hal_api unmanaged_runtime
    PRIVATE mkini machinetalk_proto_cc)
  target_link_libraries(
    managed_hal INTERFACE hal_api runtime_api) # TODO: Try to separate the
                                               # runtime_api TARGET out
  target_link_libraries(hal_module PRIVATE managed_hal)

  target_compile_definitions(managed_hal INTERFACE "RTAPI")
  target_compile_definitions(unmanaged_hal PUBLIC "ULAPI")

  export_rtapi_symbols(TARGET hal_module)

  set_target_properties(
    hal_api PROPERTIES PUBLIC_HEADER "${PUBLIC_HEADER_FILES_BUILD}"
                       PUBLIC_INCLUDE_DIRECTORY "${PUBLIC_INCLUDE_DIRECTORY}")

  set_target_properties(
    unmanaged_hal
    PROPERTIES SOVERSION 0
               PREFIX "libmkh"
               VERSION ${CMAKE_PROJECT_VERSION}
               POSITION_INDEPENDENT_CODE TRUE)

  set_target_properties(
    hal_module
    PROPERTIES OUTPUT_NAME "hal"
               PREFIX "mod"
               LIBRARY_OUTPUT_DIRECTORY
               ${MACHINEKIT_HAL_MANAGED_MODULE_OUTPUT_DIRECTORY})

  configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/MachinekitHALUnmanaged-HALComponent.cmake.in
    ${MACHINEKIT_HAL_CMAKE_PACKAGE_OUTPUT_DIRECTORY}/MachinekitHALUnmanaged-HALComponent.cmake
    NO_SOURCE_PERMISSIONS
    @ONLY)

  configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/MachinekitHALManaged-HALComponent.cmake.in
    ${MACHINEKIT_HAL_CMAKE_PACKAGE_OUTPUT_DIRECTORY}/MachinekitHALManaged-HALComponent.cmake
    NO_SOURCE_PERMISSIONS
    @ONLY)

  install(
    TARGETS unmanaged_hal
    EXPORT machinekit_hal_unmanaged_hal
    LIBRARY DESTINATION "${MACHINEKIT_HAL_LIBRARY_DIRECTORY}"
            COMPONENT MachinekitHAL_Library_Unmanaged_HAL_Libraries
            NAMELINK_COMPONENT MachinekitHAL_Library_Unmanaged_HAL_Namelinks
    PUBLIC_HEADER DESTINATION "${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/hal"
                  COMPONENT MachinekitHAL_Library_Unmanaged_HAL_Headers)

  # Managed_hal TARGET is an INTERFACE (virtual) library which has not any
  # installable artifacts, but we want to be able to call install(EXPORT) later
  install(
    TARGETS managed_hal
    EXPORT machinekit_hal_managed_hal
    COMPONENT MachinekitHAL_Library_Managed_HAL_Libraries)

  install(
    TARGETS hal_api
    EXPORT machinekit_hal_hal_api
    PUBLIC_HEADER DESTINATION "${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/hal"
                  COMPONENT MachinekitHAL_Library_HAL_API_Headers)

  install(TARGETS hal_module
          LIBRARY DESTINATION "${MACHINEKIT_HAL_MANAGED_MODULE_DIRECTORY}"
                  COMPONENT MachinekitHAL_Managed_Module_HAL_Components)

  install(
    EXPORT machinekit_hal_unmanaged_hal
    DESTINATION ${MACHINEKIT_HAL_CMAKE_PACKAGE_DIRECTORY}
    NAMESPACE "${MACHINEKIT_HAL_NAMESPACE}::"
    FILE "MachinekitHALUnmanagedHALTarget.cmake"
    COMPONENT MachinekitHAL_Library_Unmanaged_HAL_Package_Exports)

  install(
    EXPORT machinekit_hal_managed_hal
    DESTINATION ${MACHINEKIT_HAL_CMAKE_PACKAGE_DIRECTORY}
    NAMESPACE "${MACHINEKIT_HAL_NAMESPACE}::"
    FILE "MachinekitHALManagedHALTarget.cmake"
    COMPONENT MachinekitHAL_Library_Managed_HAL_Package_Exports)

  install(
    EXPORT machinekit_hal_hal_api
    DESTINATION ${MACHINEKIT_HAL_CMAKE_PACKAGE_DIRECTORY}
    NAMESPACE "${MACHINEKIT_HAL_NAMESPACE}::"
    FILE "MachinekitHALHALAPITarget.cmake"
    COMPONENT MachinekitHAL_Library_HAL_API_Package_Exports)

  install(
    FILES
      "${MACHINEKIT_HAL_CMAKE_PACKAGE_OUTPUT_DIRECTORY}/MachinekitHALUnmanaged-HALComponent.cmake"
    DESTINATION ${MACHINEKIT_HAL_CMAKE_PACKAGE_DIRECTORY}
    COMPONENT MachinekitHAL_Library_Unmanaged_HAL_Package_Exports)

  install(
    FILES
      "${MACHINEKIT_HAL_CMAKE_PACKAGE_OUTPUT_DIRECTORY}/MachinekitHALManaged-HALComponent.cmake"
    DESTINATION ${MACHINEKIT_HAL_CMAKE_PACKAGE_DIRECTORY}
    COMPONENT MachinekitHAL_Library_Managed_HAL_Package_Exports)

  export(
    EXPORT machinekit_hal_unmanaged_hal
    NAMESPACE "${MACHINEKIT_HAL_NAMESPACE}::"
    FILE "${MACHINEKIT_HAL_CMAKE_PACKAGE_OUTPUT_DIRECTORY}/MachinekitHALUnmanagedHALTarget.cmake"
  )

  export(
    EXPORT machinekit_hal_managed_hal
    NAMESPACE "${MACHINEKIT_HAL_NAMESPACE}::"
    FILE "${MACHINEKIT_HAL_CMAKE_PACKAGE_OUTPUT_DIRECTORY}/MachinekitHALManagedHALTarget.cmake"
  )

  export(
    EXPORT machinekit_hal_hal_api
    NAMESPACE "${MACHINEKIT_HAL_NAMESPACE}::"
    FILE "${MACHINEKIT_HAL_CMAKE_PACKAGE_OUTPUT_DIRECTORY}/MachinekitHALHALAPITarget.cmake"
  )

  cpack_add_component(
    MachinekitHAL_Library_Unmanaged_HAL_Libraries
    GROUP MachinekitHAL_Library_HAL
    DEPENDS MachinekitHAL_Library_Proto_Libraries)
  cpack_add_component(MachinekitHAL_Library_Managed_HAL_Libraries
                      GROUP MachinekitHAL_Library_HAL)
  cpack_add_component(
    MachinekitHAL_Library_Unmanaged_HAL_Namelinks
    GROUP MachinekitHAL_Library_HAL_Development
    DEPENDS MachinekitHAL_Library_Unmanaged_HAL_Libraries)
  cpack_add_component(
    MachinekitHAL_Library_Unmanaged_HAL_Headers
    GROUP MachinekitHAL_Library_HAL_Development
    DEPENDS MachinekitHAL_Library_Unmanaged_HAL_Libraries)
  cpack_add_component(MachinekitHAL_Library_HAL_API_Headers
                      GROUP MachinekitHAL_Library_HAL_Development)
  cpack_add_component(
    MachinekitHAL_Library_Unmanaged_HAL_Package_Exports
    GROUP MachinekitHAL_Library_HAL_Development
    DEPENDS MachinekitHAL_Library_Unmanaged_HAL_Libraries
            MachinekitHAL_Library_HAL_Headers
            MachinekitHAL_Library_Unmanaged_HAL_Namelinks)
  cpack_add_component(MachinekitHAL_Library_Managed_HAL_Package_Exports
                      GROUP MachinekitHAL_Library_HAL_Development)
  cpack_add_component(
    MachinekitHAL_Library_HAL_API_Package_Exports
    GROUP MachinekitHAL_Library_HAL_Development
    DEPENDS MachinekitHAL_Library_HAL_API_Headers)
  cpack_add_component(MachinekitHAL_Managed_Module_HAL_Components
                      GROUP MachinekitHAL_Managed_Module_HAL)

  # Specification of artifacts placement in package tree
  cpack_add_component_group(MachinekitHAL_Library_HAL
                            PARENT_GROUP MachinekitHAL_Package_Base_Libraries)
  cpack_add_component_group(
    MachinekitHAL_Library_HAL_Development
    PARENT_GROUP MachinekitHAL_Package_Base_Libraries_Development)
  cpack_add_component_group(
    MachinekitHAL_Managed_Module_HAL
    PARENT_GROUP MachinekitHAL_Package_Base_Managed_Modules_Components)

  add_subdirectory(test)
endif()
