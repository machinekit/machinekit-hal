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

option(BUILD_EXPORT_PACKAGE_LIBRARY "Built the Export package library." TRUE)

if(BUILD_EXPORT_PACKAGE_LIBRARY)
  include(CMakePackageConfigHelpers)

  write_basic_package_version_file(
    "${MACHINEKIT_HAL_CMAKE_PACKAGE_OUTPUT_DIRECTORY}/Machinekit-HALVersion.cmake"
    VERSION "${CMAKE_PROJECT_VERSION}"
    COMPATIBILITY SameMajorVersion)

  configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Machinekit-HALConfig.cmake.in
    "${MACHINEKIT_HAL_CMAKE_PACKAGE_OUTPUT_DIRECTORY}/Machinekit-HALConfig.cmake"
    INSTALL_DESTINATION "${MACHINEKIT_HAL_CMAKE_PACKAGE_DIRECTORY}")

  install(
    FILES
      "${MACHINEKIT_HAL_CMAKE_PACKAGE_OUTPUT_DIRECTORY}/Machinekit-HALConfig.cmake"
      "${MACHINEKIT_HAL_CMAKE_PACKAGE_OUTPUT_DIRECTORY}/Machinekit-HALVersion.cmake"
    DESTINATION ${MACHINEKIT_HAL_CMAKE_PACKAGE_DIRECTORY})
endif()
