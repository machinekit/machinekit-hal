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

# This does not work and will be a toughter nut
option(BUILD_MESSAGEBUS "Build the messagebus executable." FALSE)
if(BUILD_MESSAGEBUS)
  add_executable(messagebus)

  set(SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/messagebus.cc
                   ${CMAKE_CURRENT_SOURCE_DIR}/src/rtproxy.cc)

  target_sources(messagebus PRIVATE ${SOURCE_FILES})

  target_include_directories(
    messagebus PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

  pkg_check_modules(UUID "uuid" REQUIRED IMPORTED_TARGET)

  # Runtime_api just for config.h header?
  target_link_libraries(
    messagebus
    PRIVATE runtime_api
            machinetalk
            mkini
            hal
            machinekit_protoc
            machinekit_proto_cc
            syslog_async
            PkgConfig::UUID)

  target_compile_definitions(messagebus PRIVATE "PB_FIELD_32BIT")

  install(TARGETS messagebus
          RUNTIME DESTINATION "${MACHINEKIT_HAL_EXECUTABLE_DIRECTORY}"
                  COMPONENT MachinekitHAL_Executable_MessageBus_Binaries)

  cpack_add_component(
    MachinekitHAL_Executable_MessageBus_Binaries
    GROUP MachinekitHAL_Executable_MessageBus
    DEPENDS MachinekitHAL_Library_HAL_Libraries
            MachinekitHAL_Library_Proto_Libraries
            MachinekitHAL_Library_Machinetalk_Libraries
            MachinekitHAL_Library_MKini_Libraries
            MachinekitHAL_Library_SysLog_Async_Libraries)

  # Specification of artifacts placement in package tree
  cpack_add_component_group(MachinekitHAL_Executable_MessageBus
                            PARENT_GROUP MachinekitHAL_Package_Base_Executables)
endif()
