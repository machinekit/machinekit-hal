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

if(NOT DEFINED SYS_IO_AVAILABLE)
  if(NOT DEFINED check_include_files)
    include(CheckIncludeFiles)
  endif()

  check_include_files("sys/io.h" SYS_IO_AVAILABLE)
endif()

if(SYS_IO_AVAILABLE)
  option(BUILD_HAL_AX5214H_MANAGED_DRIVER_MODULE
         "Built the AX5214H HAL managed driver module." TRUE)

  if(BUILD_HAL_AX5214H_MANAGED_DRIVER_MODULE)
    include(MachinekitHALSymbolVisibility)

    add_library(hal_ax5214h MODULE)

    set(SOURCE_FILES src/hal_ax5214h.c)

    target_sources(hal_ax5214h PRIVATE ${SOURCE_FILES})

    target_link_libraries(hal_ax5214h PRIVATE hal_api runtime_api)

    export_rtapi_symbols(TARGET hal_ax5214h)

    target_compile_definitions(hal_ax5214h PRIVATE "RTAPI")

    set_target_properties(
      hal_ax5214h
      PROPERTIES PREFIX "mod"
                 LIBRARY_OUTPUT_DIRECTORY
                 "${MACHINEKIT_HAL_MANAGED_MODULE_OUTPUT_DIRECTORY}")

    install(
      TARGETS hal_ax5214h
      EXPORT machinekit_hal_managed_driver_module_hal_ax5214h
      LIBRARY DESTINATION "${MACHINEKIT_HAL_MANAGED_MODULE_DIRECTORY}"
              COMPONENT MachinekitHAL_Managed_Driver_Module_HAL_ax5214h)
  endif()
endif()
