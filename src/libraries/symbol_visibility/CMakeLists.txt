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

option(BUILD_SYMBOL_VISIBILITY_CMAKE_MODULE
       "Built the Symbol Visibility CMake Module." TRUE)

if(BUILD_SYMBOL_VISIBILITY_CMAKE_MODULE)

  set(SOURCE_FILE
      ${CMAKE_CURRENT_SOURCE_DIR}/src/MachinekitHALSymbolVisibilityFunction.cmake
  )

  configure_file(
    ${SOURCE_FILE}
    "${MACHINEKIT_HAL_CMAKE_PACKAGE_OUTPUT_DIRECTORY}/MachinekitHALSymbolVisibilityFunction.cmake"
    NO_SOURCE_PERMISSIONS
    COPYONLY)

  install(
    FILES
      "${MACHINEKIT_HAL_CMAKE_PACKAGE_OUTPUT_DIRECTORY}/MachinekitHALSymbolVisibilityFunction.cmake"
    DESTINATION "${MACHINEKIT_HAL_CMAKE_PACKAGE_DIRECTORY}"
    COMPONENT MachinekitHAL_Library_Symbol_Visibility_CMake_Functions)

  cpack_add_component(MachinekitHAL_Library_Symbol_Visibility_CMake_Functions
                      GROUP MachinekitHAL_Library_Symbol_Visibility_Development)

  # Specification of artifacts placement in package tree
  cpack_add_component_group(
    MachinekitHAL_Library_Symbol_Visibility_Development
    PARENT_GROUP MachinekitHAL_Package_Base_Libraries_Development)

endif()
