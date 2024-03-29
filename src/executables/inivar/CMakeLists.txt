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

option(BUILD_INIVAR_EXECUTABLE "Build the INIvar executable." TRUE)

if(BUILD_INIVAR_EXECUTABLE)
  add_executable(inivar)

  set(SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/inivar.cc)

  target_sources(inivar PRIVATE ${SOURCE_FILES})

  target_link_libraries(inivar PRIVATE mkini runtime_api)

  install(TARGETS inivar
          RUNTIME DESTINATION "${MACHINEKIT_HAL_EXECUTABLE_DIRECTORY}"
                  COMPONENT MachinekitHAL_Executable_INIvar_Binaries)

  cpack_add_component(
    MachinekitHAL_Executable_INIvar_Binaries
    GROUP MachinekitHAL_Executable_INIvar
    DEPENDS MachinekitHAL_Library_MKini_Libraries)

  # Specification of artifacts placement in package tree
  cpack_add_component_group(MachinekitHAL_Executable_INIvar
                            PARENT_GROUP MachinekitHAL_Package_Base_Executables)
endif()
