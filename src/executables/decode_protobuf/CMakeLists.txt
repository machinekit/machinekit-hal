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

option(BUILD_DECODE_PROTOBUF_EXECUTABLE "Build the Decode Protobuf executable."
       TRUE)

if(BUILD_DECODE_PROTOBUF_EXECUTABLE)
  add_executable(decode_protobuf)

  set(SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/pb.c)

  target_sources(decode_protobuf PRIVATE ${SOURCE_FILES})

  install(TARGETS decode_protobuf
          RUNTIME DESTINATION "${MACHINEKIT_HAL_EXECUTABLE_DIRECTORY}"
                  COMPONENT MachinekitHAL_Executable_Decode_Protobuf_Binaries)

  cpack_add_component(MachinekitHAL_Executables_Decode_Protobuf_Binaries
                      GROUP MachinekitHAL_Executable_Decode_Protobuf)

  # Specification of artifacts placement in package tree
  cpack_add_component_group(MachinekitHAL_Executable_Decode_Protobuf
                            PARENT_GROUP MachinekitHAL_Package_Base_Executables)
endif()
