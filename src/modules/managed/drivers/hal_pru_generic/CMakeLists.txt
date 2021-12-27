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

if(CMAKE_SYSTEM_PROCESSOR STREQUAL "arm")
  option(BUILD_HAL_PRU_GENERIC_MANAGED_DRIVER_MODULE
         "Built the PRU Generic HAL managed driver module." TRUE)
  if(BUILD_HAL_PRU_GENERIC_MANAGED_DRIVER_MODULE)

    cmake_minimum_required(VERSION 3.22)

    project(
      HAL-PRU-generic
      VERSION 1
      DESCRIPTION "TI PRU generic driver for Machinekit-HAL."
      HOMEPAGE_URL "https://machinekit.io"
      LANGUAGES C ASM-PRU)

    # ~~~
    # The FIRMWARE portion ###
    # ~~~
    add_executable(pru_generic)
    add_executable(pru_generic_bbai)
    add_executable(pru_generic_direct_io_only)
    add_executable(pru_generic_bbai_direct_io_only)
    add_executable(pru_decamux)
    add_executable(pru_generic_debug)
    add_executable(pru_generic_bbai_debug)
    add_executable(pru_generic_direct_io_only_debug)
    add_executable(pru_generic_bbai_direct_io_only_debug)
    add_executable(pru_decamux_debug)
    add_library(pru_empty_resource_table OBJECT)

    # ~~~
    # The MODULE portion ###
    # ~~~
    add_library(hal_pru_generic MODULE)

    # ~~~
    # The FIRMWARE portion ###
    # ~~~
    set(PRU_FIRMWARE_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/firmware/src/pru_decamux.p
        ${CMAKE_CURRENT_SOURCE_DIR}/firmware/src/pru_deltasigma.p
        ${CMAKE_CURRENT_SOURCE_DIR}/firmware/src/pru_encoder.p
        ${CMAKE_CURRENT_SOURCE_DIR}/firmware/src/pru_generic_bbai_direct_io_only.p
        ${CMAKE_CURRENT_SOURCE_DIR}/firmware/src/pru_generic_bbai.p
        ${CMAKE_CURRENT_SOURCE_DIR}/firmware/src/pru_generic_direct_io_only.p
        ${CMAKE_CURRENT_SOURCE_DIR}/firmware/src/pru_generic.p
        ${CMAKE_CURRENT_SOURCE_DIR}/firmware/src/pru_init_ecap.p
        ${CMAKE_CURRENT_SOURCE_DIR}/firmware/src/pru_init_iep.p
        ${CMAKE_CURRENT_SOURCE_DIR}/firmware/src/pru_pwm.p
        ${CMAKE_CURRENT_SOURCE_DIR}/firmware/src/pru_pwmread.p
        ${CMAKE_CURRENT_SOURCE_DIR}/firmware/src/pru_read.p
        ${CMAKE_CURRENT_SOURCE_DIR}/firmware/src/pru_stepdir.p
        ${CMAKE_CURRENT_SOURCE_DIR}/firmware/src/pru_updown.p
        ${CMAKE_CURRENT_SOURCE_DIR}/firmware/src/pru_wait_ecap.p
        ${CMAKE_CURRENT_SOURCE_DIR}/firmware/src/pru_wait.p
        ${CMAKE_CURRENT_SOURCE_DIR}/firmware/src/pru_write.p)

    set(PRU_EMPTY_RESOURCE_TABLE_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/firmware/src/generate_empty_resource_table.c
    )

    get_target_property(PRU_DEFINES_PUBLIC_HEADER_FILES pru_defines
                        PUBLIC_HEADER)

    # ~~~
    # The MODULE portion ###
    # ~~~
    set(HAL_PRU_GENERIC_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/encoder.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/hal_pru_generic.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/pwmgen.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/pwmread.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/stepgen.c)

    # ~~~
    # The FIRMWARE and MODULE portions both ###
    # ~~~
    set(PRU_PUBLIC_HEADER_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/include/hal_pru_generic/pru_tasks.h)

    # ~~~
    # The MODULE portion ###
    # ~~~
    set(HAL_PRU_GENERIC_PRIVATE_HEADER_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/beaglebone_pinmap.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/hal_pru_generic.h)

    # ~~~
    # The FIRMWARE portion ###
    # ~~~
    set(PRU_EMPTY_RESOURCE_TABLE_LINKER
        ${CMAKE_CURRENT_SOURCE_DIR}/firmware/src/linker.map)

    # ~~~
    # The FIRMWARE portion ###
    # ~~~
    # The version of TI PRU Assembler, which is needed for Machinekit-HAL's
    # HAL_PRU_Generic AL managed driver, doesn not support the -I flag
    # (specification of include directories) available in later versions of PASM
    #
    # Work around this by copying all sources to a "build" temporary directory
    # in a BINARY tree
    set(firmware_build_dir "${CMAKE_CURRENT_BINARY_DIR}/build/firmware")
    file(MAKE_DIRECTORY ${firmware_build_dir})

    set(FIRMWARE_PRU_GENERIC_SOURCE ${firmware_build_dir}/pru_generic.p)
    set(FIRMWARE_PRU_GENERIC_BBAI_SOURCE
        ${firmware_build_dir}/pru_generic_bbai.p)
    set(FIRMWARE_PRU_GENERIC_DIRECT_IO_ONLY_SOURCE
        ${firmware_build_dir}/pru_generic_direct_io_only.p)
    set(FIRMWARE_PRU_GENERIC_BBAI_DIRECT_IO_ONLY_SOURCE
        ${firmware_build_dir}/pru_generic_bbai_direct_io_only.p)
    set(FIRMWARE_PRU_DECAMUX_SOURCE ${firmware_build_dir}/pru_decamux.p)

    foreach(source_file IN
            ITEMS ${PRU_FIRMWARE_SOURCES} ${PRU_PUBLIC_HEADER_FILES}
                  ${PRU_DEFINES_PUBLIC_HEADER_FILES})
      cmake_path(GET source_file FILENAME source_file_filename)
      set(firmware_source_file "${firmware_build_dir}/${source_file_filename}")
      configure_file(${source_file} ${firmware_source_file}
                     USE_SOURCE_PERMISSIONS COPYONLY)
    endforeach()

    # ~~~
    # The FIRMWARE portion ###
    # ~~~
    #[[
    # TODO: The current implementation of FIRMWARE build is a bit
    # broken or at least not a CMake-ish. The '_debug' targets produce
    # the '.dbg' files, but the other ones produce a '.bin' file
    # and a '.bin.elf' file both.
    # Usually targets in CMake produce only one file.
    # Better solution would be to create three targets, one for the '.bin'
    # output, one for the '.dbg' output and one for the '.bin.elf'
    # The flags to produce the '.bin' or '.dbg' files also need to be
    # specified explixitly now. The usual practise is differentiate between
    # outputs by target type, however everything is build as an executable
    # target so far.
    ]]
    target_sources(pru_generic PRIVATE ${FIRMWARE_PRU_GENERIC_SOURCE})
    target_sources(pru_generic_bbai PRIVATE ${FIRMWARE_PRU_GENERIC_BBAI_SOURCE})
    target_sources(pru_generic_direct_io_only
                   PRIVATE ${FIRMWARE_PRU_GENERIC_DIRECT_IO_ONLY_SOURCE})
    target_sources(pru_generic_bbai_direct_io_only
                   PRIVATE ${FIRMWARE_PRU_GENERIC_BBAI_DIRECT_IO_ONLY_SOURCE})
    target_sources(pru_decamux PRIVATE ${FIRMWARE_PRU_DECAMUX_SOURCE})
    target_sources(pru_generic_debug PRIVATE ${FIRMWARE_PRU_GENERIC_SOURCE})
    target_sources(pru_generic_bbai_debug
                   PRIVATE ${FIRMWARE_PRU_GENERIC_BBAI_SOURCE})
    target_sources(pru_generic_direct_io_only_debug
                   PRIVATE ${FIRMWARE_PRU_GENERIC_DIRECT_IO_ONLY_SOURCE})
    target_sources(pru_generic_bbai_direct_io_only_debug
                   PRIVATE ${FIRMWARE_PRU_GENERIC_BBAI_DIRECT_IO_ONLY_SOURCE})
    target_sources(pru_decamux_debug PRIVATE ${FIRMWARE_PRU_DECAMUX_SOURCE})
    target_sources(pru_empty_resource_table
                   PRIVATE ${PRU_EMPTY_RESOURCE_TABLE_SOURCES})

    # ~~~
    # The MODULE portion ###
    # ~~~
    target_sources(
      hal_pru_generic
      PRIVATE ${HAL_PRU_GENERIC_SOURCES}
              ${HAL_PRU_GENERIC_PRIVATE_HEADER_SOURCES}
              ${PRU_PUBLIC_HEADER_FILES})

    target_include_directories(
      hal_pru_generic
      PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

    # ~~~
    # The FIRMWARE portion ###
    # ~~~
    set(pru_little_endian_binary "-b")
    set(pru_pview_debug_file "-d")

    foreach(processed_target IN
            ITEMS "pru_generic" "pru_generic_bbai" "pru_generic_direct_io_only"
                  "pru_generic_bbai_direct_io_only" "pru_decamux")
      add_custom_command(
        TARGET ${processed_target}
        POST_BUILD
        COMMAND
          ${CMAKE_OBJCOPY} "-I" "binary" "-O" "elf32-little" "--rename-section"
          ".data=.text" "--add-section"
          ".resource_table=$<TARGET_OBJECTS:pru_empty_resource_table>"
          "$<TARGET_FILE:${processed_target}>.bin"
          "${CMAKE_CURRENT_BINARY_DIR}/$<TARGET_FILE_NAME:${processed_target}>$<CONFIG>.tmp"
        COMMAND
          ${CMAKE_OBJCOPY} "-I" "elf32-little" "--set-section-flags"
          ".text=code,alloc,load"
          "${CMAKE_CURRENT_BINARY_DIR}/$<TARGET_FILE_NAME:${processed_target}>$<CONFIG>.tmp"
        COMMAND
          ${CMAKE_LINKER} "-n" "--accept-unknown-input-arch"
          "${CMAKE_CURRENT_BINARY_DIR}/$<TARGET_FILE_NAME:${processed_target}>$<CONFIG>.tmp"
          "-T" "${PRU_EMPTY_RESOURCE_TABLE_LINKER}" "-o"
          "$<TARGET_FILE:${processed_target}>.bin.elf"
        VERBATIM)

      add_dependencies(${processed_target} pru_empty_resource_table)

      target_compile_options(${processed_target}
                             PRIVATE "${pru_little_endian_binary}")

      set_target_properties(
        ${processed_target}
        PROPERTIES
          RUNTIME_OUTPUT_DIRECTORY
          "${MACHINEKIT_HAL_ARTIFACTS_MOUNTPOINT_DIRECTORY}/${CMAKE_INSTALL_LIBDIR}/${MACHINEKIT_HAL_PACKAGE_PREFIX_PATH}/prubin"
      )
    endforeach()

    foreach(
      processed_target IN
      ITEMS "pru_generic_debug" "pru_generic_bbai_debug"
            "pru_generic_direct_io_only_debug"
            "pru_generic_bbai_direct_io_only_debug" "pru_decamux_debug")

      string(REPLACE "_debug" "" output_name "${processed_target}")

      target_compile_options(${processed_target}
                             PRIVATE "${pru_pview_debug_file}")

      set_target_properties(
        ${processed_target}
        PROPERTIES
          RUNTIME_OUTPUT_DIRECTORY
          "${MACHINEKIT_HAL_ARTIFACTS_MOUNTPOINT_DIRECTORY}/${CMAKE_INSTALL_LIBDIR}/${MACHINEKIT_HAL_PACKAGE_PREFIX_PATH}/prubin"
          OUTPUT_NAME "${output_name}")
    endforeach()

    # ~~~
    # The MODULE portion ###
    # ~~~
    include(MachinekitHALSymbolVisibilityFunction)

    target_link_libraries(
      hal_pru_generic PRIVATE managed_hal managed_runtime user_pci runtime_math
                              pru_app_loader pru_defines)

    export_rtapi_symbols(TARGET hal_pru_generic)

    set_target_properties(
      hal_pru_generic
      PROPERTIES PREFIX "mod" LIBRARY_OUTPUT_DIRECTORY
                              ${MACHINEKIT_HAL_MANAGED_MODULE_OUTPUT_DIRECTORY})

    # ~~~
    # The MODULE portion ###
    # ~~~
    install(
      TARGETS hal_pru_generic
      LIBRARY DESTINATION "${MACHINEKIT_HAL_MANAGED_MODULE_DIRECTORY}"
              COMPONENT MachinekitHAL_Managed_Module_HAL_PRU_Generic_Drivers)

    # ~~~
    # The FIRMWARE portion ###
    # ~~~
    install(
      DIRECTORY
        "${MACHINEKIT_HAL_ARTIFACTS_MOUNTPOINT_DIRECTORY}/${CMAKE_INSTALL_LIBDIR}/${MACHINEKIT_HAL_PACKAGE_PREFIX_PATH}/prubin"
      DESTINATION
        "${CMAKE_INSTALL_LIBDIR}/${MACHINEKIT_HAL_PACKAGE_PREFIX_PATH}"
      # The noun 'firmware' is uncountable. The plural of 'firmware' is
      # 'firmware'. Thus this name match the used pattern.
      COMPONENT MachinekitHAL_Managed_Module_HAL_PRU_Generic_Firmware)

    cpack_add_component(
      MachinekitHAL_Managed_Module_HAL_PRU_Generic_Drivers
      GROUP MachinekitHAL_Managed_Module_HAL_PRU_Generic
      DEPENDS MachinekitHAL_Library_PRU_App_Loader_Libraries
              MachinekitHAL_Library_User_PCI_Libraries
              MachinekitHAL_Library_Runtime_Math_Libraries)

    cpack_add_component(MachinekitHAL_Managed_Module_HAL_PRU_Generic_Firmware
                        GROUP MachinekitHAL_Managed_Module_HAL_PRU_Generic)

    # Specification of artifacts placement in package tree
    cpack_add_component_group(
      MachinekitHAL_Managed_Module_HAL_PRU_Generic
      PARENT_GROUP MachinekitHAL_Package_Base_Managed_Modules_Drivers)
  endif()
endif()
