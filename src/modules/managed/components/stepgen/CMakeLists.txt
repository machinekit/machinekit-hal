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

option(BUILD_STEPGEN_MANAGED_MODULE "Built the Stepgen HAL managed module."
       TRUE)

if(BUILD_STEPGEN_MANAGED_MODULE)
  include(MachinekitHALSymbolVisibilityFunction)

  add_library(stepgen MODULE)
  add_library(${MACHINEKIT_HAL_NAMESPACE}::stepgen ALIAS stepgen)

  set(SOURCE_FILES src/stepgen.c)

  target_sources(stepgen PRIVATE ${SOURCE_FILES})

  target_link_libraries(stepgen PRIVATE managed_hal managed_runtime runtime_math)

  export_rtapi_symbols(TARGET stepgen)

  set_target_properties(
    stepgen
    PROPERTIES PREFIX "mod" LIBRARY_OUTPUT_DIRECTORY
                            "${MACHINEKIT_HAL_MANAGED_MODULE_OUTPUT_DIRECTORY}")

  install(
    TARGETS stepgen
    EXPORT machinekit_hal_managed_module_stepgen
    LIBRARY DESTINATION "${MACHINEKIT_HAL_MANAGED_MODULE_DIRECTORY}"
            COMPONENT MachinekitHAL_Managed_Module_Stepgen_Components)

  cpack_add_component(
    MachinekitHAL_Managed_Module_Stepgen_Components
    GROUP MachinekitHAL_Managed_Module_Stepgen
    DEPENDS MachinekitHAL_Library_Runtime_Math_Libraries)

  # Specification of artifacts placement in package tree
  cpack_add_component_group(
    MachinekitHAL_Managed_Module_Stepgen
    PARENT_GROUP MachinekitHAL_Package_Base_Managed_Modules_Components)
endif()
