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

if(CMAKE_SYSTEM_PROCESSOR MATCHES "^arm.*")
  option(BUILD_HOSTMOT2_SOC_OL_MANAGED_DRIVER_MODULE
         "Built the Hostmot2 SoC OL HAL managed driver module." TRUE)

  if(BUILD_HOSTMOT2_SOC_OL_MANAGED_DRIVER_MODULE)
    include(MachinekitHALSymbolVisibility)

    add_library(hostmot2_soc_ol MODULE)

    set(SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/hm2_soc_ol.c)

    set(PRIVATE_HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/hm2_soc_ol.h)

    target_sources(hostmot2_soc_ol PRIVATE ${SOURCE_FILES}
                                           ${PRIVATE_HEADER_FILES})

    target_link_libraries(
      hostmot2_soc_ol PRIVATE runtime_api hal_api hostmot2_api hostmot2_bitfile
                              user_pci)

    target_compile_definitions(hostmot2_soc_ol PRIVATE "RTAPI")

    export_rtapi_symbols(TARGET hostmot2_soc_ol)

    set_target_properties(
      hostmot2_soc_ol
      PROPERTIES OUTPUT_NAME "hm2_soc_ol"
                 PREFIX "mod"
                 LIBRARY_OUTPUT_DIRECTORY
                 ${MACHINEKIT_HAL_MANAGED_MODULE_OUTPUT_DIRECTORY})

    install(
      TARGETS hostmot2_soc_ol
      LIBRARY DESTINATION "${MACHINEKIT_HAL_MANAGED_MODULE_DIRECTORY}"
              COMPONENT MachinekitHAL_Managed_Module_Hostmot2_SoC_OL_Drivers)

    cpack_add_component(
      MachinekitHAL_Managed_Module_Hostmot2_SoC_OL_Drivers
      GROUP MachinekitHAL_Managed_Module_Hostmot2_SoC_OL
      DEPENDS MachinekitHAL_Managed_Module_Hostmot2_Core_Drivers
              MachinekitHAL_Library_User_PCI_Libraries
              MachinekitHAL_Library_Hostmot_Bitfile_Libraries)

    # Specification of artifacts placement in package tree
    cpack_add_component_group(
      MachinekitHAL_Managed_Module_Hostmot2_SoC_OL
      PARENT_GROUP MachinekitHAL_Package_Base_Managed_Modules_Drivers)
  endif()
endif()
