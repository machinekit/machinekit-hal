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

option(BUILD_SIGGEN_MANAGED_MODULE "Built the Siggen HAL managed module." TRUE)

if(BUILD_SIGGEN_MANAGED_MODULE)
  include(MachinekitHALSymbolVisibilityFunction)

  add_library(siggen MODULE)
  add_library(${MACHINEKIT_HAL_NAMESPACE}::siggen ALIAS siggen)

  set(SOURCE_FILES src/siggen.c)

  target_sources(siggen PRIVATE ${SOURCE_FILES})

  target_link_libraries(siggen PRIVATE managed_hal managed_runtime runtime_math)

  export_rtapi_symbols(TARGET siggen)

  set_target_properties(
    siggen
    PROPERTIES PREFIX "mod" LIBRARY_OUTPUT_DIRECTORY
                            "${MACHINEKIT_HAL_MANAGED_MODULE_OUTPUT_DIRECTORY}")

  install(TARGETS siggen
          LIBRARY DESTINATION "${MACHINEKIT_HAL_MANAGED_MODULE_DIRECTORY}"
                  COMPONENT MachinekitHAL_Managed_Module_Siggen_Components)

  cpack_add_component(MachinekitHAL_Managed_Module_Siggen_Components
                      GROUP MachinekitHAL_Managed_Module_Siggen)

  # Specification of artifacts placement in package tree
  cpack_add_component_group(
    MachinekitHAL_Managed_Module_Siggen
    PARENT_GROUP MachinekitHAL_Package_Base_Managed_Modules_Components)
endif()
