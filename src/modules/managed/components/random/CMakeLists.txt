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

# TODO: Full of errors, will need deeper thought
#[[
option(BUILD_RANDOM_MANAGED_MODULE "Built the Random HAL managed module." TRUE)

if(BUILD_PWMGENV2_MANAGED_MODULE)
  include(MachinekitHALSymbolVisibilityFunction)

  add_library(random MODULE)
  add_library(${MACHINEKIT_HAL_NAMESPACE}::random ALIAS random)

  set(SOURCE_FILES src/random.c)

  target_sources(random PRIVATE ${SOURCE_FILES})

  target_link_libraries(random PRIVATE managed_hal managed_runtime)

  export_rtapi_symbols(TARGET random)

  set_target_properties(
    random
    PROPERTIES PREFIX "mod" LIBRARY_OUTPUT_DIRECTORY
                            "${MACHINEKIT_HAL_MANAGED_MODULE_OUTPUT_DIRECTORY}")

  install(
    TARGETS random
    EXPORT machinekit_hal_managed_module_random
    LIBRARY DESTINATION "${MACHINEKIT_HAL_MANAGED_MODULE_DIRECTORY}"
            COMPONENT MachinekitHAL_Managed_Random)
endif()
]]
