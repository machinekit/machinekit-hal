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
  include(MachinekitHALSymbolVisibility)

  if(NOT DEFINED SYS_IO_AVAILABLE)
    check_include_files("sys/io.h" SYS_IO_AVAILABLE)
  endif()

  add_library(runtime_api INTERFACE)
  add_library(${MACHINEKIT_HAL_NAMESPACE}::runtime_api ALIAS runtime_api)
  add_library(runtime SHARED)
  add_library(${MACHINEKIT_HAL_NAMESPACE}::runtime ALIAS runtime)
  # Technically not a library, but a managed module
  add_library(runtime_module MODULE)
  add_library(${MACHINEKIT_HAL_NAMESPACE}::runtime_module ALIAS runtime_module)

  find_package(Threads REQUIRED)

  set(SOURCE_FILES
      ${CMAKE_CURRENT_SOURCE_DIR}/src/rtapi.c
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

  set(PUBLIC_HEADER_FILES
      ${CMAKE_CURRENT_SOURCE_DIR}/include/runtime/multiframe_flag.h
      ${CMAKE_CURRENT_SOURCE_DIR}/include/runtime/multiframe.h
      ${CMAKE_CURRENT_SOURCE_DIR}/include/runtime/ring.h
      ${CMAKE_CURRENT_SOURCE_DIR}/include/runtime/rtapi_app.h
      ${CMAKE_CURRENT_SOURCE_DIR}/include/runtime/rtapi_atomics.h
      ${CMAKE_CURRENT_SOURCE_DIR}/include/runtime/rtapi_bitops.h
      ${CMAKE_CURRENT_SOURCE_DIR}/include/runtime/rtapi_bool.h
      ${CMAKE_CURRENT_SOURCE_DIR}/include/runtime/rtapi_common.h
      ${CMAKE_CURRENT_SOURCE_DIR}/include/runtime/rtapi_errno.h
      ${CMAKE_CURRENT_SOURCE_DIR}/include/runtime/rtapi_exception.h
      ${CMAKE_CURRENT_SOURCE_DIR}/include/runtime/rtapi_export.h
      ${CMAKE_CURRENT_SOURCE_DIR}/include/runtime/rtapi_flavor.h
      ${CMAKE_CURRENT_SOURCE_DIR}/include/runtime/rtapi_global.h
      ${CMAKE_CURRENT_SOURCE_DIR}/include/runtime/rtapi.h
      ${CMAKE_CURRENT_SOURCE_DIR}/include/runtime/rtapi_heap.h
      ${CMAKE_CURRENT_SOURCE_DIR}/include/runtime/rtapi_heap_private.h
      ${CMAKE_CURRENT_SOURCE_DIR}/include/runtime/rtapi_hexdump.h
      ${CMAKE_CURRENT_SOURCE_DIR}/include/runtime/rtapi_int.h
      ${CMAKE_CURRENT_SOURCE_DIR}/include/runtime/rtapi_limits.h
      ${CMAKE_CURRENT_SOURCE_DIR}/include/runtime/rtapi_mbarrier.h
      ${CMAKE_CURRENT_SOURCE_DIR}/include/runtime/rtapi_mutex.h
      ${CMAKE_CURRENT_SOURCE_DIR}/include/runtime/rtapi_shmkeys.h
      ${CMAKE_CURRENT_SOURCE_DIR}/include/runtime/rtapi_stdint.h
      ${CMAKE_CURRENT_SOURCE_DIR}/include/runtime/rtapi_string.h
      ${CMAKE_CURRENT_SOURCE_DIR}/include/runtime/triple-buffer.h)

  set(RTAPI_IO_PUBLIC_HEADER
      ${CMAKE_CURRENT_SOURCE_DIR}/include/runtime/rtapi_io.h)

  set(PRIVATE_HEADER_FILES
      ${CMAKE_CURRENT_SOURCE_DIR}/src/ulapi.h
      ${CMAKE_CURRENT_SOURCE_DIR}/src/rt-preempt.h
      ${CMAKE_CURRENT_SOURCE_DIR}/src/xenomai2.h)

  target_include_directories(
    runtime_api
    INTERFACE
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      $<INSTALL_INTERFACE:${MACHINEKIT_HAL_INTERFACE_DIRECTORY}>
      $<BUILD_INTERFACE:$<TARGET_PROPERTY:rtapi_compat,INTERFACE_INCLUDE_DIRECTORIES>>
  )

  target_sources(runtime_api INTERFACE ${PUBLIC_HEADER_FILES})

  target_sources(
    runtime
    PRIVATE $<TARGET_PROPERTY:rtapi_support,INTERFACE_SOURCES>
            $<TARGET_PROPERTY:rtapi_compat,INTERFACE_SOURCES> ${SOURCE_FILES}
            ${ULAPI_FLAVOR_SOURCE_FILE} ${PRIVATE_HEADER_FILES})

  target_sources(
    runtime_module
    PRIVATE ${SOURCE_FILES} $<TARGET_PROPERTY:rtapi_support,INTERFACE_SOURCES>
            ${RT_PREEMPT_FLAVOR_SOURCE_FILE} ${PRIVATE_HEADER_FILES})

  if(SYS_IO_AVAILABLE)
    target_include_directories(
      runtime_api
      INTERFACE $<TARGET_PROPERTY:rtapi_pci,INTERFACE_INCLUDE_DIRECTORIES>)
    target_sources(runtime PRIVATE $<TARGET_PROPERTY:rtapi_pci,SOURCES>)
    target_link_libraries(runtime_api INTERFACE user_pci)
  endif()

  pkg_check_modules(CK "ck" REQUIRED IMPORTED_TARGET)
  pkg_check_modules(LIBCGROUP "libcgroup" REQUIRED IMPORTED_TARGET)
  pkg_check_modules(LIBUDEV "libudev" REQUIRED IMPORTED_TARGET)

  target_link_libraries(runtime_api INTERFACE runtime_config)
  target_link_libraries(
    runtime
    PUBLIC runtime_api runtime_memory_api
    PRIVATE ${CMAKE_DL_LIBS} syslog_async mkini)
  target_link_libraries(
    runtime_module PRIVATE runtime_api runtime_memory_api syslog_async mkini
                           Threads::Threads ${CMAKE_DL_LIBS})

  # Jesus Christ, the rtapi headers need hal headers: hal_types.h
  target_link_libraries(runtime PUBLIC hal_api)
  target_link_libraries(runtime_module PRIVATE hal_api PkgConfig::LIBCGROUP
                                               PkgConfig::LIBUDEV PkgConfig::CK)

  target_compile_definitions(runtime PUBLIC "ULAPI")
  target_compile_definitions(runtime_module PRIVATE "RTAPI" "HAVE_CK")

  check_include_files("sys/io.h" SYS_IO_AVAILABLE)

  if(SYS_IO_AVAILABLE)
    target_compile_definitions(runtime_api INTERFACE "SYS_IO_AVAILABLE")
    list(APPEND PUBLIC_HEADER_FILES ${RTAPI_IO_PUBLIC_HEADER})
    target_sources(runtime_api INTERFACE ${RTAPI_IO_PUBLIC_HEADER})
    # Add here rtapi_io.h to PUBLIC_HEADER to install
  endif()

  export_rtapi_symbols(TARGET runtime_module)

  set_target_properties(runtime_api PROPERTIES PUBLIC_HEADER
                                               "${PUBLIC_HEADER_FILES}")

  set_target_properties(
    runtime
    PROPERTIES OUTPUT_NAME "rtapi"
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
                                               "${PUBLIC_HEADER_FILES}")

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

  install(
    TARGETS
    runtime runtime_api EXPORT machinekit_hal
    LIBRARY DESTINATION "${MACHINEKIT_HAL_LIBRARY_DIRECTORY}"
            COMPONENT MachinekitHAL_Library_Base
            NAMELINK_COMPONENT MachinekitHAL_Library_Development
    PUBLIC_HEADER DESTINATION "${MACHINEKIT_HAL_INTERFACE_DIRECTORY}"
                  COMPONENT MachinekitHAL_Library_Development)

  install(
    TARGETS runtime_module
    EXPORT machinekit_hal_managed_module
    LIBRARY DESTINATION "${MACHINEKIT_HAL_MANAGED_MODULE_DIRECTORY}"
            COMPONENT MachinekitHAL_Managed_Module_Base)

  install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/install/$<CONFIG>/rtapi.ini
          ${CMAKE_CURRENT_BINARY_DIR}/install/machinekit.ini
    DESTINATION "${MACHINEKIT_HAL_SYSTEM_CONFIG_FULL_INSTALL_DIRECTORY}"
    PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
    COMPONENT MachinekitHAL_Library_Base)

  add_subdirectory(test)
endif()
