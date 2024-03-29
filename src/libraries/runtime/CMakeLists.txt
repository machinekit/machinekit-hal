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

option(BUILD_RUNTIME_LIBRARY "Built the Runtime libraries." TRUE)

if(BUILD_RUNTIME_LIBRARY)
  include(MachinekitHALSymbolVisibilityFunction)

  if(NOT DEFINED SYS_IO_AVAILABLE)
    if(NOT DEFINED check_include_files)
      include(CheckIncludeFiles)
    endif()

    check_include_files("sys/io.h" SYS_IO_AVAILABLE)
  endif()

  add_library(runtime_api INTERFACE)
  # Runtime_api target should be considered as internal TARGET of the
  # Machinekit-HAL source tree and should not be exported outside of it
  # add_library(${MACHINEKIT_HAL_NAMESPACE}::runtime_api ALIAS runtime_api)
  add_library(managed_runtime INTERFACE)
  add_library(${MACHINEKIT_HAL_NAMESPACE}::managed_runtime ALIAS
              managed_runtime)
  add_library(unmanaged_runtime SHARED)
  add_library(${MACHINEKIT_HAL_NAMESPACE}::unmanaged_runtime ALIAS
              unmanaged_runtime)
  # Technically not a library, but a managed module to be loaded into
  # Machinekit-HAL's namespace of the managing process
  add_library(runtime_module MODULE)
  # Runtime_api target should be considered as internal TARGET of the
  # Machinekit-HAL source tree and should not be exported outside of it
  add_library(${MACHINEKIT_HAL_NAMESPACE}::runtime_module ALIAS runtime_module)

  find_package(Threads REQUIRED)

  set(SOURCE_FILES
      ${CMAKE_CURRENT_SOURCE_DIR}/src/rtapi.c
      # ${CMAKE_CURRENT_SOURCE_DIR}/src/rtapi_compat.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/rtapi_exception.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/rtapi_flavor.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/rtapi_flavor_hook_wrappers.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/rtapi_heap.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/rtapi_hexdump.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/rtapi_shmem.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/rtapi_task.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/rtapi_time.c)

  set(ULAPI_FLAVOR_SOURCE_FILE ${CMAKE_CURRENT_SOURCE_DIR}/src/ulapi.c)

  set(RT_PREEMPT_FLAVOR_SOURCE_FILE
      ${CMAKE_CURRENT_SOURCE_DIR}/src/rt-preempt.c)

  set(PUBLIC_INCLUDE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include")

  set(PUBLIC_HEADER_FILES_BUILD
      ${PUBLIC_INCLUDE_DIRECTORY}/runtime/multiframe_flag.h
      ${PUBLIC_INCLUDE_DIRECTORY}/runtime/multiframe.h
      ${PUBLIC_INCLUDE_DIRECTORY}/runtime/ring.h
      ${PUBLIC_INCLUDE_DIRECTORY}/runtime/rtapi_app.h
      ${PUBLIC_INCLUDE_DIRECTORY}/runtime/rtapi_atomics.h
      ${PUBLIC_INCLUDE_DIRECTORY}/runtime/rtapi_bitops.h
      ${PUBLIC_INCLUDE_DIRECTORY}/runtime/rtapi_bool.h
      ${PUBLIC_INCLUDE_DIRECTORY}/runtime/rtapi_common.h
      ${PUBLIC_INCLUDE_DIRECTORY}/runtime/rtapi_errno.h
      ${PUBLIC_INCLUDE_DIRECTORY}/runtime/rtapi_exception.h
      ${PUBLIC_INCLUDE_DIRECTORY}/runtime/rtapi_export.h
      ${PUBLIC_INCLUDE_DIRECTORY}/runtime/rtapi_flavor.h
      ${PUBLIC_INCLUDE_DIRECTORY}/runtime/rtapi_global.h
      ${PUBLIC_INCLUDE_DIRECTORY}/runtime/rtapi.h
      ${PUBLIC_INCLUDE_DIRECTORY}/runtime/rtapi_heap.h
      ${PUBLIC_INCLUDE_DIRECTORY}/runtime/rtapi_heap_private.h
      ${PUBLIC_INCLUDE_DIRECTORY}/runtime/rtapi_hexdump.h
      ${PUBLIC_INCLUDE_DIRECTORY}/runtime/rtapi_int.h
      ${PUBLIC_INCLUDE_DIRECTORY}/runtime/rtapi_limits.h
      ${PUBLIC_INCLUDE_DIRECTORY}/runtime/rtapi_mbarrier.h
      ${PUBLIC_INCLUDE_DIRECTORY}/runtime/rtapi_mutex.h
      ${PUBLIC_INCLUDE_DIRECTORY}/runtime/rtapi_shmkeys.h
      ${PUBLIC_INCLUDE_DIRECTORY}/runtime/rtapi_stdint.h
      ${PUBLIC_INCLUDE_DIRECTORY}/runtime/rtapi_string.h
      ${PUBLIC_INCLUDE_DIRECTORY}/runtime/triple-buffer.h)

  set(PUBLIC_HEADER_FILES_BUILD_INSTALL
      ${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/runtime/multiframe_flag.h
      ${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/runtime/multiframe.h
      ${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/runtime/ring.h
      ${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/runtime/rtapi_app.h
      ${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/runtime/rtapi_atomics.h
      ${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/runtime/rtapi_bitops.h
      ${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/runtime/rtapi_bool.h
      ${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/runtime/rtapi_common.h
      ${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/runtime/rtapi_errno.h
      ${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/runtime/rtapi_exception.h
      ${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/runtime/rtapi_export.h
      ${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/runtime/rtapi_flavor.h
      ${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/runtime/rtapi_global.h
      ${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/runtime/rtapi.h
      ${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/runtime/rtapi_heap.h
      ${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/runtime/rtapi_heap_private.h
      ${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/runtime/rtapi_hexdump.h
      ${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/runtime/rtapi_int.h
      ${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/runtime/rtapi_limits.h
      ${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/runtime/rtapi_mbarrier.h
      ${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/runtime/rtapi_mutex.h
      ${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/runtime/rtapi_shmkeys.h
      ${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/runtime/rtapi_stdint.h
      ${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/runtime/rtapi_string.h
      ${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/runtime/triple-buffer.h)

  set(RTAPI_IO_PUBLIC_HEADER_BUILD
      ${PUBLIC_INCLUDE_DIRECTORY}/runtime/rtapi_io.h)

  set(RTAPI_IO_PUBLIC_HEADER_INSTALL
      ${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/runtime/rtapi_io.h)

  set(PRIVATE_HEADER_FILES
      ${CMAKE_CURRENT_SOURCE_DIR}/src/ulapi.h
      ${CMAKE_CURRENT_SOURCE_DIR}/src/rt-preempt.h
      ${CMAKE_CURRENT_SOURCE_DIR}/src/xenomai2.h)

  target_include_directories(
    runtime_api
    INTERFACE $<BUILD_INTERFACE:${PUBLIC_INCLUDE_DIRECTORY}>
              $<INSTALL_INTERFACE:${MACHINEKIT_HAL_INTERFACE_DIRECTORY}>)

  check_include_files("sys/io.h" SYS_IO_AVAILABLE)

  if(SYS_IO_AVAILABLE)
    list(APPEND PUBLIC_HEADER_FILES_BUILD ${RTAPI_IO_PUBLIC_HEADER_BUILD})
    list(APPEND PUBLIC_HEADER_FILES_INSTALL ${RTAPI_IO_PUBLIC_HEADER_INSTALL})

    target_compile_definitions(runtime_api INTERFACE "SYS_IO_AVAILABLE")
    # TODO: Add here rtapi_io.h to PUBLIC_HEADER to install

    target_sources(
      runtime_api
      INTERFACE "$<BUILD_INTERFACE:${RTAPI_IO_PUBLIC_HEADER_BUILD}>"
                "$<INSTALL_INTERFACE:${RTAPI_IO_PUBLIC_HEADER_INSTALL}>")
  endif()

  target_sources(
    runtime_api INTERFACE "$<BUILD_INTERFACE:${PUBLIC_HEADER_FILES_BUILD}>"
                          "$<INSTALL_INTERFACE:${PUBLIC_HEADER_FILES_INSTALL}>")

  target_sources(
    unmanaged_runtime
    PRIVATE $<TARGET_PROPERTY:rtapi_support,INTERFACE_SOURCES> ${SOURCE_FILES}
            ${ULAPI_FLAVOR_SOURCE_FILE} ${PRIVATE_HEADER_FILES})

  target_sources(
    runtime_module
    PRIVATE "${SOURCE_FILES}"
            "$<TARGET_PROPERTY:rtapi_support,INTERFACE_SOURCES>"
            "${RT_PREEMPT_FLAVOR_SOURCE_FILE}" "${PRIVATE_HEADER_FILES}")

  pkg_check_modules(CK "ck" REQUIRED IMPORTED_TARGET)
  pkg_check_modules(LIBCGROUP "libcgroup" REQUIRED IMPORTED_TARGET)

  target_link_libraries(runtime_api INTERFACE runtime_config)
  target_link_libraries(managed_runtime INTERFACE runtime_api rtapi_compat)
  target_link_libraries(
    unmanaged_runtime
    PUBLIC runtime_api runtime_memory_api rtapi_compat mkini
    PRIVATE ${CMAKE_DL_LIBS} syslog_async)

  if(SYS_IO_AVAILABLE)
    target_link_libraries(unmanaged_runtime PUBLIC unmanaged_rtapi_pci)
  endif()

  target_link_libraries(
    runtime_module
    PRIVATE rtapi_compat
            managed_runtime
            runtime_memory_api
            syslog_async
            mkini
            Threads::Threads
            ${CMAKE_DL_LIBS})

  # Jesus Christ, the rtapi headers need hal headers: hal_types.h TODO!
  target_link_libraries(unmanaged_runtime PUBLIC hal_api)
  target_link_libraries(runtime_module PRIVATE hal_api PkgConfig::LIBCGROUP
                                               PkgConfig::CK)

  target_compile_definitions(unmanaged_runtime PUBLIC "ULAPI")
  target_compile_definitions(managed_runtime INTERFACE "RTAPI")
  target_compile_definitions(runtime_module PRIVATE "HAVE_CK")

  export_rtapi_symbols(TARGET runtime_module)

  set_target_properties(
    runtime_api
    PROPERTIES PUBLIC_HEADER "${PUBLIC_HEADER_FILES_BUILD}"
               PUBLIC_INCLUDE_DIRECTORY "${PUBLIC_INCLUDE_DIRECTORY}")

  set_target_properties(
    unmanaged_runtime
    PROPERTIES PREFIX "libmkh"
               SOVERSION 0
               VERSION ${CMAKE_PROJECT_VERSION}
               PRIVATE_HEADER "${PRIVATE_HEADER_FILES}"
               POSITION_INDEPENDENT_CODE TRUE)

  set_target_properties(
    runtime_module
    PROPERTIES OUTPUT_NAME "runtime"
               PREFIX "mod"
               LIBRARY_OUTPUT_DIRECTORY
               ${MACHINEKIT_HAL_MANAGED_MODULE_OUTPUT_DIRECTORY})

  set_target_properties(runtime_api PROPERTIES PUBLIC_HEADERS
                                               "${PUBLIC_HEADER_FILES_BUILD}")

  string(TIMESTAMP RIGHT_NOW)
  string(
    UUID
    MACHINEKIT_HAL_RUNTIME_GUUID
    NAMESPACE
    "a42c8c6b-4025-4f83-ba28-dad21114744a"
    NAME
    "${RIGHT_NOW}"
    TYPE
    SHA1)

  configure_file(
    src/machinekit.ini.in
    ${CMAKE_CURRENT_BINARY_DIR}/build/machinekit.ini.configure @ONLY
    NEWLINE_STYLE UNIX)

  file(
    GENERATE
    OUTPUT ${MACHINEKIT_HAL_SYSTEM_CONFIG_OUTPUT_DIRECTORY}/machinekit.ini
    INPUT ${CMAKE_CURRENT_BINARY_DIR}/build/machinekit.ini.configure
    NEWLINE_STYLE UNIX FILE_PERMISSIONS OWNER_READ GROUP_READ WORLD_READ)

  set(MACHINEKIT_HAL_RUNTIME_GUUID "a42c8c6b-4025-4f83-ba28-dad21114744a")

  configure_file(
    src/machinekit.ini.in ${CMAKE_CURRENT_BINARY_DIR}/install/machinekit.ini
    @ONLY NEWLINE_STYLE UNIX FILE_PERMISSIONS OWNER_READ GROUP_READ WORLD_READ)

  # TODO: Fix the variable structure / check what is needed and what can be
  # safely removed
  set(RTAPI_INI_RTAPI_MSGD_EXECUTABLE "$<TARGET_FILE:msgd>")
  set(RTAPI_INI_RTAPI_APP_EXECUTABLE "$<TARGET_FILE:rtapi_app>")
  set(RTAPI_INI_HAL_HOME "${MACHINEKIT_HAL_ARTIFACTS_MOUNTPOINT_DIRECTORY}")
  set(RTAPI_INI_RTAPI_RTLIB_DIR
      "${MACHINEKIT_HAL_ARTIFACTS_MOUNTPOINT_DIRECTORY}/${CMAKE_INSTALL_LIBDIR}/${MACHINEKIT_HAL_PACKAGE_PREFIX_PATH}/module/managed"
  )
  set(RTAPI_INI_RTAPI_LIBEXEC_DIR
      "${MACHINEKIT_HAL_INTERNAL_EXECUTABLE_OUTPUT_DIRECTORY}")
  set(RTAPI_INI_RTAPI_BIN_DIR "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

  configure_file(
    src/rtapi.ini.in ${CMAKE_CURRENT_BINARY_DIR}/build/rtapi.ini.configure @ONLY
    NEWLINE_STYLE UNIX)

  file(
    GENERATE
    OUTPUT ${MACHINEKIT_HAL_SYSTEM_CONFIG_OUTPUT_DIRECTORY}/rtapi.ini
    INPUT ${CMAKE_CURRENT_BINARY_DIR}/build/rtapi.ini.configure
    NEWLINE_STYLE UNIX FILE_PERMISSIONS OWNER_READ GROUP_READ WORLD_READ)

  set(RTAPI_INI_RTAPI_MSGD_EXECUTABLE
      "${MACHINEKIT_HAL_INTERNAL_EXECUTABLE_FULL_INSTALL_DIRECTORY}/$<TARGET_FILE_NAME:msgd>"
  )
  set(RTAPI_INI_RTAPI_APP_EXECUTABLE
      "${MACHINEKIT_HAL_INTERNAL_EXECUTABLE_FULL_INSTALL_DIRECTORY}/$<TARGET_FILE_NAME:rtapi_app>"
  )
  set(RTAPI_INI_HAL_HOME "${CMAKE_INSTALL_PREFIX}") # TODO: Really??
  set(RTAPI_INI_RTAPI_RTLIB_DIR
      "${MACHINEKIT_HAL_LIBRARY_FULL_INSTALL_DIRECTORY}/${MACHINEKIT_HAL_PACKAGE_PREFIX_PATH}/module/managed"
  )
  set(RTAPI_INI_RTAPI_LIBEXEC_DIR
      "${MACHINEKIT_HAL_INTERNAL_EXECUTABLE_FULL_INSTALL_DIRECTORY}")
  set(RTAPI_INI_RTAPI_BIN_DIR
      "${MACHINEKIT_HAL_EXECUTABLE_FULL_INSTALL_DIRECTORY}")

  configure_file(
    src/rtapi.ini.in ${CMAKE_CURRENT_BINARY_DIR}/install/rtapi.ini.configure
    @ONLY NEWLINE_STYLE UNIX)

  file(
    GENERATE
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/install/$<CONFIG>/rtapi.ini
    INPUT ${CMAKE_CURRENT_BINARY_DIR}/install/rtapi.ini.configure
    NEWLINE_STYLE UNIX FILE_PERMISSIONS OWNER_READ GROUP_READ WORLD_READ)

  configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/MachinekitHALUnmanaged-RuntimeComponent.cmake.in
    ${MACHINEKIT_HAL_CMAKE_PACKAGE_OUTPUT_DIRECTORY}/MachinekitHALUnmanaged-RuntimeComponent.cmake
    NO_SOURCE_PERMISSIONS
    @ONLY)

  configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/MachinekitHALManaged-RuntimeComponent.cmake.in
    ${MACHINEKIT_HAL_CMAKE_PACKAGE_OUTPUT_DIRECTORY}/MachinekitHALManaged-RuntimeComponent.cmake
    NO_SOURCE_PERMISSIONS
    @ONLY)

  install(
    TARGETS unmanaged_runtime
    EXPORT machinekit_hal_unmanaged_runtime
    LIBRARY DESTINATION "${MACHINEKIT_HAL_LIBRARY_DIRECTORY}"
            COMPONENT MachinekitHAL_Library_Unmanaged_Runtime_Libraries
            NAMELINK_COMPONENT MachinekitHAL_Library_Unmanaged_Runtime_Namelinks
    PUBLIC_HEADER DESTINATION "${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/runtime"
                  COMPONENT MachinekitHAL_Library_Unmanaged_Runtime_Headers)

  # Managed_runtime TARGET is an INTERFACE (virtual) library which has not any
  # installable artifacts, but we want to be able to call install(EXPORT) later
  install(
    TARGETS managed_runtime
    EXPORT machinekit_hal_managed_runtime
    COMPONENT MachinekitHAL_Library_Managed_Runtime_Libraries)

  install(
    TARGETS runtime_api
    EXPORT machinekit_hal_runtime_api
    PUBLIC_HEADER DESTINATION "${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/runtime"
                  COMPONENT MachinekitHAL_Library_Runtime_API_Headers)

  install(TARGETS runtime_module
          LIBRARY DESTINATION "${MACHINEKIT_HAL_MANAGED_MODULE_DIRECTORY}"
                  COMPONENT MachinekitHAL_Managed_Module_Runtime_Components)

  install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/install/$<CONFIG>/rtapi.ini
          ${CMAKE_CURRENT_BINARY_DIR}/install/machinekit.ini
    DESTINATION "${MACHINEKIT_HAL_SYSTEM_CONFIG_FULL_INSTALL_DIRECTORY}"
    PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
    COMPONENT MachinekitHAL_Library_Runtime_Config_Files)

  install(
    EXPORT machinekit_hal_unmanaged_runtime
    DESTINATION ${MACHINEKIT_HAL_CMAKE_PACKAGE_DIRECTORY}
    NAMESPACE "${MACHINEKIT_HAL_NAMESPACE}::"
    FILE "MachinekitHALUnmanagedRuntimeTarget.cmake"
    COMPONENT MachinekitHAL_Library_Unmanaged_Runtime_Package_Exports)

  install(
    EXPORT machinekit_hal_managed_runtime
    DESTINATION ${MACHINEKIT_HAL_CMAKE_PACKAGE_DIRECTORY}
    NAMESPACE "${MACHINEKIT_HAL_NAMESPACE}::"
    FILE "MachinekitHALManagedRuntimeTarget.cmake"
    COMPONENT MachinekitHAL_Library_Managed_Runtime_Package_Exports)

  install(
    EXPORT machinekit_hal_runtime_api
    DESTINATION ${MACHINEKIT_HAL_CMAKE_PACKAGE_DIRECTORY}
    NAMESPACE "${MACHINEKIT_HAL_NAMESPACE}::"
    FILE "MachinekitHALRuntimeAPITarget.cmake"
    COMPONENT MachinekitHAL_Library_Runtime_API_Package_Exports)

  install(
    FILES
      "${MACHINEKIT_HAL_CMAKE_PACKAGE_OUTPUT_DIRECTORY}/MachinekitHALUnmanaged-RuntimeComponent.cmake"
    DESTINATION ${MACHINEKIT_HAL_CMAKE_PACKAGE_DIRECTORY}
    COMPONENT MachinekitHAL_Library_Unmanaged_Runtime_Package_Exports)

  install(
    FILES
      "${MACHINEKIT_HAL_CMAKE_PACKAGE_OUTPUT_DIRECTORY}/MachinekitHALManaged-RuntimeComponent.cmake"
    DESTINATION ${MACHINEKIT_HAL_CMAKE_PACKAGE_DIRECTORY}
    COMPONENT MachinekitHAL_Library_Managed_Runtime_Package_Exports)

  export(
    EXPORT machinekit_hal_unmanaged_runtime
    NAMESPACE "${MACHINEKIT_HAL_NAMESPACE}::"
    FILE "${MACHINEKIT_HAL_CMAKE_PACKAGE_OUTPUT_DIRECTORY}/MachinekitHALUnmanagedRuntimeTarget.cmake"
  )

  export(
    EXPORT machinekit_hal_managed_runtime
    NAMESPACE "${MACHINEKIT_HAL_NAMESPACE}::"
    FILE "${MACHINEKIT_HAL_CMAKE_PACKAGE_OUTPUT_DIRECTORY}/MachinekitHALManagedRuntimeTarget.cmake"
  )

  export(
    EXPORT machinekit_hal_runtime_api
    NAMESPACE "${MACHINEKIT_HAL_NAMESPACE}::"
    FILE "${MACHINEKIT_HAL_CMAKE_PACKAGE_OUTPUT_DIRECTORY}/MachinekitHALRuntimeAPITarget.cmake"
  )

  cpack_add_component(
    MachinekitHAL_Library_Unmanaged_Runtime_Libraries
    GROUP MachinekitHAL_Library_Runtime
    DEPENDS MachinekitHAL_Library_SysLog_Async_Libraries
            MachinekitHAL_Library_MKini_Libraries
            MachinekitHAL_Library_Runtime_Memory_API_Libraries
            MachinekitHAL_Library_RTAPI_PCI_Libraries
            MachinekitHAL_Library_Runtime_Config_Files)
  cpack_add_component(MachinekitHAL_Library_Managed_Runtime_Libraries
                      GROUP MachinekitHAL_Library_Runtime)
  cpack_add_component(
    MachinekitHAL_Library_Unmanaged_Runtime_Namelinks
    GROUP MachinekitHAL_Library_Runtime_Development
    DEPENDS MachinekitHAL_Library_Unmanaged_Runtime_Libraries)
  cpack_add_component(
    MachinekitHAL_Library_Unmanaged_Runtime_Headers
    GROUP MachinekitHAL_Library_Runtime_Development
    DEPENDS MachinekitHAL_Library_Unmanaged_Runtime_Libraries)
  cpack_add_component(MachinekitHAL_Library_Runtime_API_Headers
                      GROUP MachinekitHAL_Library_Runtime_Development)
  cpack_add_component(
    MachinekitHAL_Library_Unmanaged_Runtime_Package_Exports
    GROUP MachinekitHAL_Library_Runtime_Development
    DEPENDS MachinekitHAL_Library_Unmanaged_Runtime_Libraries
            MachinekitHAL_Library_Unmanaged_Runtime_Headers
            MachinekitHAL_Library_Unmanaged_Runtime_Namelinks)
  cpack_add_component(
    MachinekitHAL_Library_Runtime_API_Package_Exports
    GROUP MachinekitHAL_Library_Runtime_Development
    DEPENDS MachinekitHAL_Library_Runtime_API_Headers)
  cpack_add_component(
    MachinekitHAL_Library_Managed_Runtime_Package_Exports
    GROUP MachinekitHAL_Library_Runtime_Development
    DEPENDS MachinekitHAL_Library_Managed_Runtime_Libraries)
  cpack_add_component(
    MachinekitHAL_Library_Runtime_Config_Files
    GROUP MachinekitHAL_Library_Runtime
    DEPENDS MachinekitHAL_Library_Unmanaged_Runtime_Libraries)
  cpack_add_component(MachinekitHAL_Managed_Module_Runtime_Components
                      GROUP MachinekitHAL_Managed_Module_Runtime)

  # Specification of artifacts placement in package tree
  cpack_add_component_group(MachinekitHAL_Library_Runtime
                            PARENT_GROUP MachinekitHAL_Package_Base_Libraries)
  cpack_add_component_group(
    MachinekitHAL_Library_Runtime_Development
    PARENT_GROUP MachinekitHAL_Package_Base_Libraries_Development)
  cpack_add_component_group(
    MachinekitHAL_Managed_Module_Runtime
    PARENT_GROUP MachinekitHAL_Package_Base_Managed_Modules_Components)

  add_subdirectory(test)
endif()
