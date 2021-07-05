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

option(BUILD_MKINI_LIBRARY "Built the MKINI library." TRUE)

if(BUILD_MKINI_LIBRARY)
  add_library(mkini SHARED)
  add_library(${MACHINEKIT_HAL_NAMESPACE}::mkini ALIAS mkini)

  set(SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/inifile.cc)
  set(PUBLIC_HEADER_FILES
      ${CMAKE_CURRENT_SOURCE_DIR}/include/mkini/mk-inifile.h
      ${CMAKE_CURRENT_SOURCE_DIR}/include/mkini/mk-inifile.hh)

  target_sources(mkini PRIVATE ${SOURCE_FILES} ${PUBLIC_HEADER_FILES})

  # TODO: Needed just because of the config.h??
  target_link_libraries(mkini PRIVATE runtime_api)

  target_include_directories(
    mkini PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                 $<INSTALL_INTERFACE:${MACHINEKIT_HAL_INTERFACE_DIRECTORY}>)

  set_target_properties(
    mkini
    PROPERTIES SOVERSION 0
               PREFIX "libmkh"
               VERSION ${CMAKE_PROJECT_VERSION}
               PUBLIC_HEADER "${PUBLIC_HEADER_FILES}"
               POSITION_INDEPENDENT_CODE TRUE)
  install(
    TARGETS mkini
    EXPORT machinekit_hal_mkini
    LIBRARY DESTINATION "${MACHINEKIT_HAL_LIBRARY_DIRECTORY}"
            COMPONENT MachinekitHAL_Library_MKini_Libraries
            NAMELINK_COMPONENT MachinekitHAL_Library_MKini_Namelinks
    PUBLIC_HEADER DESTINATION "${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/mkini"
                  COMPONENT MachinekitHAL_Library_MKini_Headers)

  cpack_add_component(MachinekitHAL_Library_MKini_Libraries
                      GROUP MachinekitHAL_Library_MKini)
  cpack_add_component(
    MachinekitHAL_Library_MKini_Namelinks
    GROUP MachinekitHAL_Library_MKini_Development
    DEPENDS MachinekitHAL_Library_MKini_Libraries)
  cpack_add_component(
    MachinekitHAL_Library_MKini_Headers
    GROUP MachinekitHAL_Library_MKini_Development
    DEPENDS MachinekitHAL_Library_MKini_Libraries)

  # Specification of artifacts placement in package tree
  cpack_add_component_group(MachinekitHAL_Library_MKini
                            PARENT_GROUP MachinekitHAL_Package_Base_Libraries)
  cpack_add_component_group(
    MachinekitHAL_Library_MKini_Development
    PARENT_GROUP MachinekitHAL_Package_Base_Libraries_Development)
endif()
