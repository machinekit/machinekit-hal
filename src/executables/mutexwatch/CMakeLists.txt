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

option(BUILD_MUTEXWATCH_EXECUTABLE "Built the Mutexwatch executable." TRUE)

if(BUILD_MUTEXWATCH_EXECUTABLE)
  add_executable(mutexwatch)

  set(SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/mutexwatch.c)

  target_sources(mutexwatch PRIVATE ${SOURCE_FILES})

  # TODO: Use FindLibRT CMake module or not?
  find_library(RT "rt" REQUIRED)

  target_link_libraries(mutexwatch PRIVATE runtime hal runtime_memory_api ${RT})

  target_compile_definitions(mutexwatch PRIVATE "ULAPI")

  install(TARGETS mutexwatch
          RUNTIME DESTINATION "${MACHINEKIT_HAL_EXECUTABLE_DIRECTORY}"
                  COMPONENT MachinekitHAL_Executable_MutexWatch_Binaries)

  cpack_add_component(
    MachinekitHAL_Executable_MutexWatch_Binaries
    GROUP MachinekitHAL_Executable_MutexWatch
    DEPENDS MachinekitHAL_Library_Runtime_Libraries
            MachinekitHAL_Library_HAL_Libraries
            MachinekitHAL_Library_Runtime_Memory_API_Libraries)

  # Specification of artifacts placement in package tree
  cpack_add_component_group(MachinekitHAL_Executable_MutexWatch
                            PARENT_GROUP MachinekitHAL_Package_Base_Executables)
endif()
