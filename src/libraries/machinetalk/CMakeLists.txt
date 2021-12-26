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

  set(PUBLIC_INCLUDE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include")

  set(PUBLIC_HEADER_FILES_BUILD
      ${PUBLIC_INCLUDE_DIRECTORY}/machinetalk/bin2ascii.hh
      ${PUBLIC_INCLUDE_DIRECTORY}/machinetalk/container.h
      ${PUBLIC_INCLUDE_DIRECTORY}/machinetalk/czmq-watch.h
      ${PUBLIC_INCLUDE_DIRECTORY}/machinetalk/inihelp.hh
      ${PUBLIC_INCLUDE_DIRECTORY}/machinetalk/json2pb.hh
      ${PUBLIC_INCLUDE_DIRECTORY}/machinetalk/ll-zeroconf.hh
      ${PUBLIC_INCLUDE_DIRECTORY}/machinetalk/mk-backtrace.h
      ${PUBLIC_INCLUDE_DIRECTORY}/machinetalk/mk-service.hh
      ${PUBLIC_INCLUDE_DIRECTORY}/machinetalk/mk-zeroconf.hh
      ${PUBLIC_INCLUDE_DIRECTORY}/machinetalk/mk-zeroconf-types.h
      ${PUBLIC_INCLUDE_DIRECTORY}/machinetalk/pb-hal.h
      ${PUBLIC_INCLUDE_DIRECTORY}/machinetalk/pbutil.hh
      ${PUBLIC_INCLUDE_DIRECTORY}/machinetalk/setup_signals.h)

  set(PUBLIC_HEADER_FILES_INSTALL
      ${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/machinetalk/bin2ascii.hh
      ${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/machinetalk/container.h
      ${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/machinetalk/czmq-watch.h
      ${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/machinetalk/inihelp.hh
      ${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/machinetalk/json2pb.hh
      ${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/machinetalk/ll-zeroconf.hh
      ${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/machinetalk/mk-backtrace.h
      ${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/machinetalk/mk-service.hh
      ${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/machinetalk/mk-zeroconf.hh
      ${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/machinetalk/mk-zeroconf-types.h
      ${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/machinetalk/pb-hal.h
      ${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/machinetalk/pbutil.hh
      ${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/machinetalk/setup_signals.h)

  # Are these headers really PUBLIC or not?
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
    PUBLIC $<BUILD_INTERFACE:${PUBLIC_INCLUDE_DIRECTORY}>
           $<INSTALL_INTERFACE:${MACHINEKIT_HAL_INTERFACE_DIRECTORY}>)

  target_compile_definitions(machinetalk PRIVATE "PB_FIELD_32BIT")

  set_target_properties(
    machinetalk
    PROPERTIES SOVERSION 0
               PREFIX "libmkh"
               VERSION ${CMAKE_PROJECT_VERSION}
               PUBLIC_HEADER "${PUBLIC_HEADER_FILES_BUILD}"
               POSITION_INDEPENDENT_CODE TRUE
               PUBLIC_INCLUDE_DIRECTORY "${PUBLIC_INCLUDE_DIRECTORY}")

  install(
    TARGETS machinetalk
    EXPORT machinekit_hal_machinetalk
    LIBRARY DESTINATION "${MACHINEKIT_HAL_LIBRARY_DIRECTORY}"
            COMPONENT MachinekitHAL_Library_Machinetalk_Libraries
            NAMELINK_COMPONENT MachinekitHAL_Library_Machinetalk_Namelinks
    PUBLIC_HEADER
      DESTINATION "${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/machinetalk"
      COMPONENT MachinekitHAL_Library_Machinetalk_Headers)

  install(
    EXPORT machinekit_hal_machinetalk
    DESTINATION ${MACHINEKIT_HAL_CMAKE_PACKAGE_DIRECTORY}
    NAMESPACE "${MACHINEKIT_HAL_NAMESPACE}::"
    FILE "MachinekitHALMachinetalkTarget.cmake"
    COMPONENT MachinekitHAL_Library_Machinetalk_Package_Exports)

  export(
    EXPORT machinekit_hal_machinetalk
    NAMESPACE "${MACHINEKIT_HAL_NAMESPACE}::"
    FILE "${MACHINEKIT_HAL_CMAKE_PACKAGE_OUTPUT_DIRECTORY}/MachinekitHALMachinetalkTarget.cmake"
  )

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
  cpack_add_component(
    MachinekitHAL_Library_Machinetalk_Package_Exports
    GROUP MachinekitHAL_Library_Machinetalk_Development
    DEPENDS MachinekitHAL_Library_Machinetalk_Libraries
            MachinekitHAL_Library_Machinetalk_Headers
            MachinekitHAL_Library_Machinetalk_Namelinks)

  # Specification of artifacts placement in package tree
  cpack_add_component_group(MachinekitHAL_Library_Machinetalk
                            PARENT_GROUP MachinekitHAL_Package_Base_Libraries)
  cpack_add_component_group(
    MachinekitHAL_Library_Machinetalk_Development
    PARENT_GROUP MachinekitHAL_Package_Base_Libraries_Development)
endif()
