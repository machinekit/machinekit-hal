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
  option(BUILD_HOSTMOT2_7I43_MANAGED_DRIVER_MODULE
         "Built the Hostmot2 7I43 HAL managed driver module." TRUE)

  if(BUILD_HOSTMOT2_7I43_MANAGED_DRIVER_MODULE)
    include(MachinekitHALSymbolVisibilityFunction)

    add_library(hostmot2_7i43 MODULE)

    set(SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/hm2_7i43.c)

    set(PRIVATE_HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/hm2_7i43.h)

    target_sources(hostmot2_7i43 PRIVATE ${SOURCE_FILES}
                                         ${PRIVATE_HEADER_FILES})

    target_link_libraries(
      hostmot2_7i43
      PRIVATE managed_hal
              managed_runtime
              hostmot2_api
              hostmot2_bitfile
              user_pci
              runtime_math
              rtapi_parport)

    export_rtapi_symbols(TARGET hostmot2_7i43)

    set_target_properties(
      hostmot2_7i43
      PROPERTIES OUTPUT_NAME "hm2_7i43"
                 PREFIX "mod"
                 LIBRARY_OUTPUT_DIRECTORY
                 ${MACHINEKIT_HAL_MANAGED_MODULE_OUTPUT_DIRECTORY})

    install(
      TARGETS hostmot2_7i43
      LIBRARY DESTINATION "${MACHINEKIT_HAL_MANAGED_MODULE_DIRECTORY}"
              COMPONENT MachinekitHAL_Managed_Module_Hostmot2_7I43_Drivers)

    cpack_add_component(
      MachinekitHAL_Managed_Module_Hostmot2_7I43_Drivers
      GROUP MachinekitHAL_Managed_Module_Hostmot2_7I43
      DEPENDS MachinekitHAL_Managed_Module_Hostmot2_Core_Drivers
              MachinekitHAL_Library_User_PCI_Libraries
              MachinekitHAL_Library_Hostmot2_Bitfile_Libraries
              MachinekitHAL_Library_Runtime_Math_Libraries)

    # Specification of artifacts placement in package tree
    cpack_add_component_group(
      MachinekitHAL_Managed_Module_Hostmot2_7I43
      PARENT_GROUP MachinekitHAL_Package_Base_Managed_Modules_Drivers)

  endif()
endif()
