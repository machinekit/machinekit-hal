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

  set(SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/shmdrvapi.c)

  set(PUBLIC_INCLUDE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include")

  set(PUBLIC_HEADER_FILES_BUILD ${PUBLIC_INCLUDE_DIRECTORY}/runtime/shmdrv.h)

  set(PUBLIC_HEADER_FILES_INSTALL
      ${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/runtime/shmdrv.h)

  target_include_directories(
    runtime_memory_api
    PUBLIC $<BUILD_INTERFACE:${PUBLIC_INCLUDE_DIRECTORY}>
           $<INSTALL_INTERFACE:${MACHINEKIT_HAL_INTERFACE_DIRECTORY}>)

  target_sources(
    runtime_memory_api
    PUBLIC "$<BUILD_INTERFACE:${PUBLIC_HEADER_FILES_BUILD}>"
           "$<INSTALL_INTERFACE:${PUBLIC_HEADER_FILES_INSTALL}>"
    PRIVATE ${SOURCE_FILES})

  find_library(RT NAMES "rt" "librt" "librt.so" "librt.so.1" REQUIRED)

  target_link_libraries(runtime_memory_api PRIVATE runtime_api ${RT})

  # This is a special case because runtime_memory_api TARGET is a direct
  # dependency of unmanaged_runtime TARGET and thus the "ULAPI" compile
  # definition cannot be transitivelly defined by linking agains the
  # unmanaged_runtime as that would create a cyclic dependency and CMake error
  #
  # TODO: Maybe there is a better solution to this mess?
  add_compile_definitions(runtime_memory_api PRIVATE "ULAPI")

  set_target_properties(
    runtime_memory_api
    PROPERTIES SOVERSION 0
               PREFIX "libmkh"
               VERSION ${CMAKE_PROJECT_VERSION}
               PUBLIC_HEADER "${PUBLIC_HEADER_FILES_BUILD}"
               POSITION_INDEPENDENT_CODE TRUE
               PUBLIC_INCLUDE_DIRECTORY "${PUBLIC_INCLUDE_DIRECTORY}")
  install(
    TARGETS runtime_memory_api
    EXPORT machinekit_hal_runtime_memory_api
    LIBRARY
      DESTINATION "${MACHINEKIT_HAL_LIBRARY_DIRECTORY}"
      COMPONENT MachinekitHAL_Library_Runtime_Memory_API_Libraries
      NAMELINK_COMPONENT MachinekitHAL_Library_Runtime_Memory_API_Namelinks
    PUBLIC_HEADER DESTINATION "${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/runtime"
                  COMPONENT MachinekitHAL_Library_Runtime_Memory_API_Headers)

  install(
    EXPORT machinekit_hal_runtime_memory_api
    DESTINATION ${MACHINEKIT_HAL_CMAKE_PACKAGE_DIRECTORY}
    NAMESPACE "${MACHINEKIT_HAL_NAMESPACE}::"
    FILE "MachinekitHALRuntimeMemoryAPITarget.cmake"
    COMPONENT MachinekitHAL_Library_Runtime_Memory_API_Package_Exports)

  export(
    EXPORT machinekit_hal_runtime_memory_api
    NAMESPACE "${MACHINEKIT_HAL_NAMESPACE}::"
    FILE "${MACHINEKIT_HAL_CMAKE_PACKAGE_OUTPUT_DIRECTORY}/MachinekitHALRuntimeMemoryAPITarget.cmake"
  )

  cpack_add_component(MachinekitHAL_Library_Runtime_Memory_API_Libraries
                      GROUP MachinekitHAL_Library_Runtime_Memory_API)
  cpack_add_component(
    MachinekitHAL_Library_Runtime_Memory_API_Namelinks
    GROUP MachinekitHAL_Library_Runtime_Memory_API_Development
    DEPENDS MachinekitHAL_Library_Runtime_Memory_API_Libraries)
  cpack_add_component(
    MachinekitHAL_Library_Runtime_Memory_API_Headers
    GROUP MachinekitHAL_Library_Runtime_Memory_API_Development
    DEPENDS MachinekitHAL_Library_Runtime_Memory_API_Libraries)
  cpack_add_component(
    MachinekitHAL_Library_Runtime_Memory_API_Package_Exports
    GROUP MachinekitHAL_Library_Runtime_Memory_API_Development
    DEPENDS MachinekitHAL_Library_Runtime_Memory_API_Libraries
            MachinekitHAL_Library_Runtime_Memory_API_Headers
            MachinekitHAL_Library_Runtime_Memory_API_Namelinks)

  # Specification of artifacts placement in package tree
  cpack_add_component_group(MachinekitHAL_Library_Runtime_Memory_API
                            PARENT_GROUP MachinekitHAL_Package_Base_Libraries)
  cpack_add_component_group(
    MachinekitHAL_Library_Runtime_Memory_API_Development
    PARENT_GROUP MachinekitHAL_Package_Base_Libraries_Development)
endif()
