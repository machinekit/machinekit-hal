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

option(BUILD_LCD_MANAGED_MODULE "Built the LCD HAL managed module." TRUE)

if(BUILD_LCD_MANAGED_MODULE)
  include(MachinekitHALSymbolVisibilityFunction)

  add_library(lcd MODULE)
  add_library(${MACHINEKIT_HAL_NAMESPACE}::lcd ALIAS lcd)

  set(SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/lcd.c)

  target_sources(lcd PRIVATE ${SOURCE_FILES})

  target_link_libraries(lcd PRIVATE managed_hal managed_runtime runtime_math)

  export_rtapi_symbols(TARGET lcd)

  set_target_properties(
    lcd
    PROPERTIES PREFIX "mod" LIBRARY_OUTPUT_DIRECTORY
                            "${MACHINEKIT_HAL_MANAGED_MODULE_OUTPUT_DIRECTORY}")

  install(TARGETS lcd
          LIBRARY DESTINATION "${MACHINEKIT_HAL_MANAGED_MODULE_DIRECTORY}"
                  COMPONENT MachinekitHAL_Managed_Module_LCD_Components)

  cpack_add_component(MachinekitHAL_Managed_Module_LCD_Components
                      GROUP MachinekitHAL_Managed_Module_LCD)

  # Specification of artifacts placement in package tree
  cpack_add_component_group(
    MachinekitHAL_Managed_Module_LCD
    PARENT_GROUP MachinekitHAL_Package_Base_Managed_Modules_Components)
endif()
