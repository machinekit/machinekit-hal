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

option(BUILD_NANOPB_DEFINITIONS_LIBRARY "Built the NanoPB definitions library."
       TRUE)

if(BUILD_NANOPB_DEFINITIONS_LIBRARY)
  add_library(nanopb_definitions SHARED)
  add_library(${MACHINEKIT_HAL_NAMESPACE}::nanopb_definitions ALIAS
              nanopb_definitions)

  set(SOURCE_FILES src/nanopb.c)

  target_sources(nanopb_definitions PRIVATE ${SOURCE_FILES})

  target_link_libraries(nanopb_definitions PRIVATE nanopb_interface runtime_api
                                                   machinetalk)

  target_compile_definitions(nanopb_definitions PRIVATE "RTAPI")

  set_target_properties(
    nanopb_definitions
    PROPERTIES SOVERSION 0
               VERSION ${CMAKE_PROJECT_VERSION}
               POSITION_INDEPENDENT_CODE TRUE)

  install(
    TARGETS nanopb_definitions
    EXPORT machinekit_hal
    LIBRARY DESTINATION "${MACHINEKIT_HAL_LIBRARY_DIRECTORY}"
            COMPONENT MachinekitHAL_Library_Base
            NAMELINK_COMPONENT MachinekitHAL_Library_Development)
endif()
