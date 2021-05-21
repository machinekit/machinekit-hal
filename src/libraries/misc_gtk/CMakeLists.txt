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

option(BUILD_MISC_GTK_LIBRARY "Built the Misc GTK library." TRUE)

if(BUILD_MISC_GTK_LIBRARY)
  add_library(miscgtk SHARED)
  add_library(${MACHINEKIT_HAL_NAMESPACE}::miscgtk ALIAS miscgtk)

  set(SOURCE_FILES src/miscgtk.c)

  target_sources(miscgtk PRIVATE ${SOURCE_FILES})
  target_include_directories(
    miscgtk PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

  pkg_check_modules(LIBGTK "gtk+-2.0" REQUIRED IMPORTED_TARGET)

  target_link_libraries(miscgtk PUBLIC PkgConfig::LIBGTK)

  set_target_properties(
    miscgtk
    PROPERTIES SOVERSION 0
               VERSION ${CMAKE_PROJECT_VERSION}
               RUNTIME_OUTPUT_DIRECTORY "${CMAKE_INSTALL_LIBDIR}"
               PUBLIC_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/include/miscgtk.h")

  install(
    TARGETS miscgtk
    EXPORT machinekit_hal
    LIBRARY DESTINATION "${MACHINEKIT_HAL_LIBRARY_DIRECTORY}"
            COMPONENT MachinekitHAL_Library_Base
            NAMELINK_COMPONENT MachinekitHAL_Library_Development
    PUBLIC_HEADER DESTINATION "${MACHINEKIT_HAL_INTERFACE_DIRECTORY}"
                  COMPONENT MachinekitHAL_Library_Development)

endif()
