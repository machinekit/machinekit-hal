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

option(BUILD_ENCDEC_EXECUTABLE "Build the Encdec executable." TRUE)

if(BUILD_ENCDEC_EXECUTABLE)
  add_executable(encdec)

  set(SOURCE_FILES src/encdec.cc)

  target_sources(encdec PRIVATE ${SOURCE_FILES})

  pkg_check_modules(PROTOBUF "protobuf" REQUIRED IMPORTED_TARGET)

  target_link_libraries(encdec PRIVATE protobuf machinetalk
                                       machinetalk_proto_cc)

  target_compile_definitions(encdec PRIVATE "PB_FIELD_32BIT")

  install(TARGETS encdec
          RUNTIME DESTINATION "${MACHINEKIT_HAL_EXECUTABLE_DIRECTORY}"
                  COMPONENT MachinekitHAL_Executable_Emcdec_Binaries)

  cpack_add_component(
    MachinekitHAL_Executable_Emcdec_Binaries
    GROUP MachinekitHAL_Executable_Emcdec
    DEPENDS MachinekitHAL_Library_Proto_Libraries
            MachinekitHAL_Library_Machinetalk_Libraries)

  # Specification of artifacts placement in package tree
  cpack_add_component_group(MachinekitHAL_Executable_Emcdec
                            PARENT_GROUP MachinekitHAL_Package_Base_Executables)
endif()
