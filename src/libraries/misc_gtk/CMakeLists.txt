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

  set(PUBLIC_INCLUDE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include")

  set(PUBLIC_HEADER_FILES_BUILD ${PUBLIC_INCLUDE_DIRECTORY}/misc_gtk/miscgtk.h)

  set(PUBLIC_HEADER_FILES_INSTALL
      ${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/misc_gtk/miscgtk.h)

  target_sources(
    miscgtk
    PUBLIC "$<BUILD_INTERFACE:${PUBLIC_HEADER_FILES_BUILD}>"
           "$<INSTALL_INTERFACE:${PUBLIC_HEADER_FILES_INSTALL}>"
    PRIVATE ${SOURCE_FILES})

  target_include_directories(
    miscgtk PUBLIC $<BUILD_INTERFACE:${PUBLIC_INCLUDE_DIRECTORY}>)

  pkg_check_modules(GTK "gtk+-2.0" REQUIRED IMPORTED_TARGET)

  target_link_libraries(miscgtk PUBLIC PkgConfig::GTK)

  set_target_properties(
    miscgtk
    PROPERTIES SOVERSION 0
               PREFIX "libmkh"
               VERSION ${CMAKE_PROJECT_VERSION}
               RUNTIME_OUTPUT_DIRECTORY "${CMAKE_INSTALL_LIBDIR}"
               PUBLIC_HEADER "${PUBLIC_HEADER_FILES_BUILD}"
               PUBLIC_INCLUDE_DIRECTORY "${PUBLIC_INCLUDE_DIRECTORY}")

  install(
    TARGETS miscgtk
    EXPORT machinekit_hal_misc_gtk
    LIBRARY DESTINATION "${MACHINEKIT_HAL_LIBRARY_DIRECTORY}"
            COMPONENT MachinekitHAL_Library_Misc_GTK_Libraries
            NAMELINK_COMPONENT MachinekitHAL_Library_Misc_GTK_Namelinks
    PUBLIC_HEADER DESTINATION "${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/misc_gtk"
                  COMPONENT MachinekitHAL_Library_Misc_GTK_Headers)

  install(
    EXPORT machinekit_hal_misc_gtk
    DESTINATION ${MACHINEKIT_HAL_CMAKE_PACKAGE_DIRECTORY}
    NAMESPACE "${MACHINEKIT_HAL_NAMESPACE}::"
    FILE "MachinekitHALMiscGTKTarget.cmake"
    COMPONENT MachinekitHAL_Library_Misc_GTK_Package_Exports)

  export(
    EXPORT machinekit_hal_misc_gtk
    NAMESPACE "${MACHINEKIT_HAL_NAMESPACE}::"
    FILE "${MACHINEKIT_HAL_CMAKE_PACKAGE_OUTPUT_DIRECTORY}/MachinekitHALMiscGTKTarget.cmake"
  )

  cpack_add_component(MachinekitHAL_Library_Misc_GTK_Libraries
                      GROUP MachinekitHAL_Library_Misc_GTK)
  cpack_add_component(
    MachinekitHAL_Library_Misc_GTK_Namelinks
    GROUP MachinekitHAL_Library_Misc_GTK_Development
    DEPENDS MachinekitHAL_Library_Misc_GTK_Libraries)
  cpack_add_component(
    MachinekitHAL_Library_Misc_GTK_Headers
    GROUP MachinekitHAL_Library_Misc_GTK_Development
    DEPENDS MachinekitHAL_Library_Misc_GTK_Libraries)
  cpack_add_component(
    MachinekitHAL_Library_Misc_GTK_Package_Exports
    GROUP MachinekitHAL_Library_Misc_GTK_Development
    DEPENDS MachinekitHAL_Library_Misc_GTK_Libraries
            MachinekitHAL_Library_Misc_GTK_Headers)

  # Specification of artifacts placement in package tree
  cpack_add_component_group(MachinekitHAL_Library_Misc_GTK
                            PARENT_GROUP MachinekitHAL_Package_Base_Libraries)
  cpack_add_component_group(
    MachinekitHAL_Library_Misc_GTK_Development
    PARENT_GROUP MachinekitHAL_Package_Base_Libraries_Development)
endif()
