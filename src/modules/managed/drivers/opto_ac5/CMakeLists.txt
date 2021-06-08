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
#[[
option(BUILD_OPTO_AC5_MANAGED_DRIVER_MODULE
       "Built the Opto AC5 HAL managed driver module." TRUE)

if(BUILD_OPTO_AC5_MANAGED_DRIVER_MODULE)
  include(MachinekitHALSymbolVisibility)

  add_library(opto_ac5 MODULE)

  set(SOURCE_FILES src/opto_ac5.c)

  set(PRIVATE_HEADER_FILES src/opto_ac5.h)

  target_sources(opto_ac5 PRIVATE ${SOURCE_FILES} ${PRIVATE_HEADER_FILES})

  target_link_libraries(opto_ac5 PRIVATE hal_api runtime_api)

  export_rtapi_symbols(TARGET opto_ac5)

  target_compile_definitions(opto_ac5 PRIVATE "RTAPI")

  set_target_properties(
    opto_ac5
    PROPERTIES PREFIX "mod" LIBRARY_OUTPUT_DIRECTORY
                            "${MACHINEKIT_HAL_MANAGED_MODULE_OUTPUT_DIRECTORY}")

  install(TARGETS opto_ac5
          LIBRARY DESTINATION "${MACHINEKIT_HAL_MANAGED_MODULE_DIRECTORY}"
                  COMPONENT MachinekitHAL_Managed_Driver_Module_Opto_AC5)
endif()
]]
