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
  option(BUILD_HAL_PRU_DEBUG_MANAGED_DRIVER_MODULE
         "Built the PRU Debug HAL managed driver module." TRUE)
  if(BUILD_HAL_PRU_DEBUG_MANAGED_DRIVER_MODULE)
    include(MachinekitHALSymbolVisibilityFunction)

    add_library(hal_pru_debug MODULE)

    set(SOURCE_FILES src/hal_prudebug.c)

    target_sources(hal_pru_debug PRIVATE ${SOURCE_FILES})

    target_link_libraries(hal_pru_debug PRIVATE managed_hal managed_runtime
                                                pru_app_loader pru_defines)

    export_rtapi_symbols(TARGET hal_pru_debug)

    set_target_properties(
      hal_pru_debug
      PROPERTIES PREFIX "mod"
                 LIBRARY_OUTPUT_DIRECTORY
                 "${MACHINEKIT_HAL_MANAGED_MODULE_OUTPUT_DIRECTORY}")

    install(
      TARGETS hal_pru_debug
      LIBRARY DESTINATION "${MACHINEKIT_HAL_MANAGED_MODULE_DIRECTORY}"
              COMPONENT MachinekitHAL_Managed_Module_HAL_PRU_Debug_Drivers)

    cpack_add_component(
      MachinekitHAL_Managed_Module_HAL_PRU_Debug_Drivers
      GROUP MachinekitHAL_Managed_Module_HAL_PRU_Debug
      DEPENDS MachinekitHAL_Library_PRU_App_Loader_Libraries)

    # Specification of artifacts placement in package tree
    cpack_add_component_group(
      MachinekitHAL_Managed_Module_HAL_PRU_Debug
      PARENT_GROUP MachinekitHAL_Package_Base_Managed_Modules_Drivers)

  endif()
endif()
