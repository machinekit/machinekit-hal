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

option(BUILD_MACHINETALK_LIBRARY "Built the Machinetalk library." TRUE)

if(BUILD_MACHINETALK_LIBRARY)
  add_library(machinetalk SHARED)
  add_library(${MACHINEKIT_HAL_NAMESPACE}::machinetalk ALIAS machinetalk)

  set(SOURCE_FILES
      ${CMAKE_CURRENT_SOURCE_DIR}/src/czmq-watch.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/inihelp.cc
      ${CMAKE_CURRENT_SOURCE_DIR}/src/json2pb.cc
      ${CMAKE_CURRENT_SOURCE_DIR}/src/ll_zeroconf_register.cc
      ${CMAKE_CURRENT_SOURCE_DIR}/src/mk_backtrace.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/mk_service.cc
      ${CMAKE_CURRENT_SOURCE_DIR}/src/mk_zeroconf.cc
      ${CMAKE_CURRENT_SOURCE_DIR}/src/pbutil.cc
      ${CMAKE_CURRENT_SOURCE_DIR}/src/setup_signals.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/zeroconf_resolve.cc)

  set(PUBLIC_HEADER_FILES
      ${CMAKE_CURRENT_SOURCE_DIR}/include/machinetalk/bin2ascii.hh
      ${CMAKE_CURRENT_SOURCE_DIR}/include/machinetalk/container.h
      ${CMAKE_CURRENT_SOURCE_DIR}/include/machinetalk/czmq-watch.h
      ${CMAKE_CURRENT_SOURCE_DIR}/include/machinetalk/inihelp.hh
      ${CMAKE_CURRENT_SOURCE_DIR}/include/machinetalk/json2pb.hh
      ${CMAKE_CURRENT_SOURCE_DIR}/include/machinetalk/ll-zeroconf.hh
      ${CMAKE_CURRENT_SOURCE_DIR}/include/machinetalk/mk-backtrace.h
      ${CMAKE_CURRENT_SOURCE_DIR}/include/machinetalk/mk-service.hh
      ${CMAKE_CURRENT_SOURCE_DIR}/include/machinetalk/mk-zeroconf.hh
      ${CMAKE_CURRENT_SOURCE_DIR}/include/machinetalk/mk-zeroconf-types.h
      ${CMAKE_CURRENT_SOURCE_DIR}/include/machinetalk/pb-hal.h
      ${CMAKE_CURRENT_SOURCE_DIR}/include/machinetalk/pbutil.hh
      ${CMAKE_CURRENT_SOURCE_DIR}/include/machinetalk/setup_signals.h)

  target_sources(machinetalk PRIVATE ${SOURCE_FILES} ${PUBLIC_HEADER_FILES})

  pkg_check_modules(AVAHICLIENT "avahi-client" REQUIRED IMPORTED_TARGET)
  pkg_check_modules(CZMQ "libczmq" REQUIRED IMPORTED_TARGET)
  pkg_check_modules(JANSSON "jansson" REQUIRED IMPORTED_TARGET)
  pkg_check_modules(UUID "uuid" REQUIRED IMPORTED_TARGET)

  target_link_libraries(
    machinetalk
    PUBLIC machinetalk_proto_cc
    PRIVATE runtime_api
            hal_api
            mkini
            syslog_async
            nanopb_interface
            PkgConfig::AVAHICLIENT
            PkgConfig::CZMQ
            PkgConfig::JANSSON
            PkgConfig::UUID)

  target_include_directories(
    machinetalk
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
           $<INSTALL_INTERFACE:${MACHINEKIT_HAL_INTERFACE_DIRECTORY}>)

  target_compile_definitions(machinetalk PRIVATE "PB_FIELD_32BIT")

  set_target_properties(
    machinetalk
    PROPERTIES SOVERSION 0
               PREFIX "libmkh"
               VERSION ${CMAKE_PROJECT_VERSION}
               PUBLIC_HEADER "${PUBLIC_HEADER_FILES}"
               POSITION_INDEPENDENT_CODE TRUE)

  install(
    TARGETS machinetalk
    EXPORT machinekit_hal_machinetalk
    LIBRARY DESTINATION "${MACHINEKIT_HAL_LIBRARY_DIRECTORY}"
            COMPONENT MachinekitHAL_Library_Machinetalk_Libraries
            NAMELINK_COMPONENT MachinekitHAL_Library_Machinetalk_Namelinks
    PUBLIC_HEADER
      DESTINATION "${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/machinetalk"
      COMPONENT MachinekitHAL_Library_Machinetalk_Headers)

  cpack_add_component(
    MachinekitHAL_Library_Machinetalk_Libraries
    GROUP MachinekitHAL_Library_Machinetalk
    DEPENDS MachinekitHAL_Library_Proto_Libraries
            MachinekitHAL_Library_MKini_Libraries
            MachinekitHAL_Library_MKini_Libraries)
  cpack_add_component(
    MachinekitHAL_Library_Machinetalk_Namelinks
    GROUP MachinekitHAL_Library_Machinetalk_Development
    DEPENDS MachinekitHAL_Library_Machinetalk_Libraries)
  cpack_add_component(
    MachinekitHAL_Library_Machinetalk_Headers
    GROUP MachinekitHAL_Library_Machinetalk_Development
    DEPENDS MachinekitHAL_Library_Machinetalk_Libraries)

  # Specification of artifacts placement in package tree
  cpack_add_component_group(MachinekitHAL_Library_Machinetalk
                            PARENT_GROUP MachinekitHAL_Package_Base_Libraries)
  cpack_add_component_group(
    MachinekitHAL_Library_Machinetalk_Development
    PARENT_GROUP MachinekitHAL_Package_Base_Libraries_Development)
endif()
