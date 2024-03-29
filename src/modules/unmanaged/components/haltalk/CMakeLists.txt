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

option(BUILD_HALTALK_UNMANAGED_MODULE "Built the Haltalk unmanaged HAL module."
       TRUE)

if(BUILD_HALTALK_UNMANAGED_MODULE)
  add_executable(haltalk)

  set(SOURCE_FILES
      ${CMAKE_CURRENT_SOURCE_DIR}/src/haltalk_bridge.cc
      ${CMAKE_CURRENT_SOURCE_DIR}/src/haltalk_command.cc
      ${CMAKE_CURRENT_SOURCE_DIR}/src/haltalk_group.cc
      ${CMAKE_CURRENT_SOURCE_DIR}/src/haltalk_introspect.cc
      ${CMAKE_CURRENT_SOURCE_DIR}/src/haltalk_main.cc
      ${CMAKE_CURRENT_SOURCE_DIR}/src/haltalk_rcomp.cc)

  set(PRIVATE_HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/haltalk.hh)

  target_sources(haltalk PRIVATE ${SOURCE_FILES} ${PRIVATE_HEADER_FILES})

  find_library(MATH NAMES "m" REQUIRED)
  find_package(Threads REQUIRED)
  pkg_check_modules(AVAHICLIENT "avahi-client" REQUIRED IMPORTED_TARGET)
  pkg_check_modules(PROTOBUF "protobuf" REQUIRED IMPORTED_TARGET)
  pkg_check_modules(JANSSON "jansson" REQUIRED IMPORTED_TARGET)
  pkg_check_modules(ZMQ "libzmq" REQUIRED IMPORTED_TARGET)
  pkg_check_modules(CZMQ "libczmq" REQUIRED IMPORTED_TARGET)
  pkg_check_modules(UUID "uuid" REQUIRED IMPORTED_TARGET)

  target_link_libraries(
    haltalk
    PRIVATE machinetalk
            machinetalk_proto_cc
            unmanaged_runtime
            unmanaged_hal
            mkini
            syslog_async
            PkgConfig::AVAHICLIENT
            PkgConfig::PROTOBUF
            PkgConfig::JANSSON
            PkgConfig::ZMQ
            PkgConfig::CZMQ
            PkgConfig::UUID
            ${MATH}
            Threads::Threads)

  install(TARGETS haltalk
          RUNTIME DESTINATION "${MACHINEKIT_HAL_UNMANAGED_MODULE_DIRECTORY}"
                  COMPONENT MachinekitHAL_Unmanaged_Module_Haltalk_Components)

  cpack_add_component(
    MachinekitHAL_Unmanaged_Module_Haltalk_Components
    GROUP MachinekitHAL_Unmanaged_Module_Haltalk
    DEPENDS MachinekitHAL_Library_Runtime_Libraries
            MachinekitHAL_Library_HAL_Libraries
            MachinekitHAL_Library_Machinetalk_Libraries
            MachinekitHAL_Library_Proto_Libraries
            MachinekitHAL_Library_MKIni_Libraries
            MachinekitHAL_Library_Syslog_Async_Libraries)

  # Specification of artifacts placement in package tree
  cpack_add_component_group(
    MachinekitHAL_Unmanaged_Module_Haltalk
    PARENT_GROUP MachinekitHAL_Package_Base_Unmanaged_Modules_Components)
endif()
