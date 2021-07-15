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

if(CMAKE_SYSTEM_PROCESSOR MATCHES "^arm.*")
  option(BUILD_MKSOCMEMIO_EXECUTABLE "Build the MKSoCMemIo executable." TRUE)

  if(BUILD_MKSOCMEMIO_EXECUTABLE)
    add_executable(mksocmemio)

    set(SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/mksocmemio.c)

    target_sources(mksocmemio PRIVATE ${SOURCE_FILES})

    target_link_libraries(mksocmemio PRIVATE runtime_api hal_api)

    target_compile_definitions(mksocmemio PRIVATE "ULAPI")

    install(TARGETS mksocmemio
            RUNTIME DESTINATION "${MACHINEKIT_HAL_EXECUTABLE_DIRECTORY}"
                    COMPONENT MachinekitHAL_Executable_MKSoCMEMIO_Binaries)

    cpack_add_component(MachinekitHAL_Executable_MKSoCMEMIO_Binaries
                        GROUP MachinekitHAL_Executable_MKSoCMEMIO)

    # Specification of artifacts placement in package tree
    cpack_add_component_group(
      MachinekitHAL_Executable_MKSoCMEMIO
      PARENT_GROUP MachinekitHAL_Package_Base_Executables)
  endif()
endif()
