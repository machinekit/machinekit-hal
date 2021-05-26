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

option(BUILD_ENCODER_MANAGED_MODULE "Built the Encoder HAL managed module."
       TRUE)

if(BUILD_ENCODER_MANAGED_MODULE)
  include(MachinekitHALSymbolVisibility)

  add_library(encoder MODULE)
  add_library(${MACHINEKIT_HAL_NAMESPACE}::encoder ALIAS encoder)

  set(SOURCE_FILES src/encoder.c)

  target_sources(encoder PRIVATE ${SOURCE_FILES})

  target_link_libraries(encoder PRIVATE hal_api runtime_api)

  export_rtapi_symbols(TARGET encoder)

  target_compile_definitions(encoder PRIVATE "RTAPI")

  set_target_properties(
    encoder
    PROPERTIES PREFIX "mod" LIBRARY_OUTPUT_DIRECTORY
                            "${MACHINEKIT_HAL_MANAGED_MODULE_OUTPUT_DIRECTORY}")

  install(
    TARGETS encoder
    EXPORT machinekit_hal_managed_module_encoder
    LIBRARY DESTINATION "${MACHINEKIT_HAL_MANAGED_MODULE_DIRECTORY}"
            COMPONENT MachinekitHAL_Managed_Module_Encoder)
endif()
