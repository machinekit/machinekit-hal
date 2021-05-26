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

# Full of errors, will need deeper thought
#[[option(BUILD_CONST_RT_MANAGED_MODULE "Built the Const-RT HAL managed module."
       FALSE)

if(BUILD_CONST_RT_MANAGED_MODULE)
  include(MachinekitHALSymbolVisibility)

  add_library(const_rt MODULE)
  add_library(${MACHINEKIT_HAL_NAMESPACE}::const-rt ALIAS const_rt)

  set(SOURCE_FILES src/const-rt.cc)

  target_sources(const_rt PRIVATE ${SOURCE_FILES})

  target_link_libraries(const_rt PRIVATE hal_api runtime_api)

  export_rtapi_symbols(TARGET const_rt)

  target_compile_definitions(const_rt PRIVATE "RTAPI")

  set_target_properties(
    const_rt
    PROPERTIES OUTPUT_NAME "const-rt"
               PREFIX "mod"
               LIBRARY_OUTPUT_DIRECTORY
               "${MACHINEKIT_HAL_MANAGED_MODULE_OUTPUT_DIRECTORY}")

  install(
    TARGETS const_rt
    EXPORT machinekit_hal_managed_module_const_rt
    LIBRARY DESTINATION "${MACHINEKIT_HAL_MANAGED_MODULE_DIRECTORY}"
            COMPONENT MachinekitHAL_Managed_Module_Const_RT)
endif()]]
