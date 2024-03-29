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

option(BUILD_JPLAN_MANAGED_MODULE "Built the JPlan HAL managed module." TRUE)

if(BUILD_JPLAN_MANAGED_MODULE)
  include(MachinekitHALSymbolVisibilityFunction)

  add_library(jplan MODULE)
  add_library(${MACHINEKIT_HAL_NAMESPACE}::jplan ALIAS jplan)

  set(SOURCE_FILES src/jplan.c)

  target_sources(jplan PRIVATE ${SOURCE_FILES})

  target_link_libraries(jplan PRIVATE managed_hal managed_runtime runtime_math
                                      machinetalk_proto_c)

  export_rtapi_symbols(TARGET jplan)

  set_target_properties(
    jplan
    PROPERTIES PREFIX "mod" LIBRARY_OUTPUT_DIRECTORY
                            "${MACHINEKIT_HAL_MANAGED_MODULE_OUTPUT_DIRECTORY}")

  install(TARGETS jplan
          LIBRARY DESTINATION "${MACHINEKIT_HAL_MANAGED_MODULE_DIRECTORY}"
                  COMPONENT MachinekitHAL_Managed_Module_JPlan_Components)

  cpack_add_component(MachinekitHAL_Managed_Module_JPlan_Components
                      GROUP MachinekitHAL_Managed_Module_JPlan)

  # Specification of artifacts placement in package tree
  cpack_add_component_group(
    MachinekitHAL_Managed_Module_JPlan
    PARENT_GROUP MachinekitHAL_Package_Base_Managed_Modules_Components)
endif()
