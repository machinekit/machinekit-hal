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
option(BUILD_PROTO_LIBRARY "Built the Proto library." TRUE)

if(BUILD_PROTO_LIBRARY)
  include("${PROJECT_SOURCE_DIR}/cmake/tools/FindProtobuf.cmake")

  add_library(machinetalk_proto_c SHARED)
  add_library(${MACHINEKIT_HAL_NAMESPACE}::machinetalk_proto_c ALIAS
              machinetalk_proto_c)
  add_library(machinetalk_proto_cc SHARED)
  add_library(${MACHINEKIT_HAL_NAMESPACE}::machinetalk_proto_cc ALIAS
              machinetalk_proto_cc)

  set(SOURCE_PROTO_FILES
      ${CMAKE_CURRENT_SOURCE_DIR}/src/machinetalk/protobuf/jplan.proto
      ${CMAKE_CURRENT_SOURCE_DIR}/src/machinetalk/protobuf/log.proto
      ${CMAKE_CURRENT_SOURCE_DIR}/src/machinetalk/protobuf/message.proto
      ${CMAKE_CURRENT_SOURCE_DIR}/src/machinetalk/protobuf/motcmds.proto
      ${CMAKE_CURRENT_SOURCE_DIR}/src/machinetalk/protobuf/object.proto
      ${CMAKE_CURRENT_SOURCE_DIR}/src/machinetalk/protobuf/preview.proto
      ${CMAKE_CURRENT_SOURCE_DIR}/src/machinetalk/protobuf/ros.proto
      ${CMAKE_CURRENT_SOURCE_DIR}/src/machinetalk/protobuf/rtapi_message.proto
      ${CMAKE_CURRENT_SOURCE_DIR}/src/machinetalk/protobuf/rtapicommand.proto
      ${CMAKE_CURRENT_SOURCE_DIR}/src/machinetalk/protobuf/sample.proto
      ${CMAKE_CURRENT_SOURCE_DIR}/src/machinetalk/protobuf/status.proto
      ${CMAKE_CURRENT_SOURCE_DIR}/src/machinetalk/protobuf/task.proto
      ${CMAKE_CURRENT_SOURCE_DIR}/src/machinetalk/protobuf/test.proto
      ${CMAKE_CURRENT_SOURCE_DIR}/src/machinetalk/protobuf/types.proto
      ${CMAKE_CURRENT_SOURCE_DIR}/src/machinetalk/protobuf/value.proto
      ${CMAKE_CURRENT_SOURCE_DIR}/src/machinetalk/protobuf/emcclass.proto
      ${CMAKE_CURRENT_SOURCE_DIR}/src/machinetalk/protobuf/canon.proto
      ${CMAKE_CURRENT_SOURCE_DIR}/src/machinetalk/protobuf/config.proto)

  set(SOURCE_NANOPB_PROTO_FILE
      ${nanopb_SOURCE_DIR}/generator/proto/nanopb.proto)

  set(PROTO_GENERATED_BASE_DIR ${CMAKE_CURRENT_BINARY_DIR})

  file(MAKE_DIRECTORY ${PROTO_GENERATED_BASE_DIR}/src)
  # file(MAKE_DIRECTORY ${PROTO_GENERATED_BASE_DIR}/c)

  target_include_directories(
    machinetalk_proto_cc
    PUBLIC $<BUILD_INTERFACE:${PROTO_GENERATED_BASE_DIR}/src>)
  target_include_directories(
    machinetalk_proto_c
    PUBLIC $<BUILD_INTERFACE:${PROTO_GENERATED_BASE_DIR}/src>)

  message("${PROTO_GENERATED_BASE_DIR}\n${CMAKE_CURRENT_BINARY_DIR}")

  pkg_check_modules(PROTOBUF "protobuf" REQUIRED IMPORTED_TARGET)

  target_link_libraries(
    machinetalk_proto_c PUBLIC nanopb_interface PkgConfig::PROTOBUF
                               protobuf-nanopb-static)
  target_link_libraries(
    machinetalk_proto_cc PUBLIC nanopb_interface nanopb_proto_cc
                                PkgConfig::PROTOBUF)

  protobuf_generate(
    LANGUAGE
    cpp
    PROTOC_OUT_DIR
    "${PROTO_GENERATED_BASE_DIR}/src"
    IMPORT_DIRS
    "${nanopb_SOURCE_DIR}/generator/proto" # "${CMAKE_CURRENT_SOURCE_DIR}/src"
    # PROTOC_PLUGIN
    # "--plugin=protoc-gen-nanopb=${nanopb_SOURCE_DIR}/generator/protoc-gen-nanopb"
    # PROTOC_ARGUMENTS "--nanopb_opt=--generated-include-format='#include <%s>'
    # --extension=.pb --options-file=${CMAKE_CURRENT_SOURCE_DIR}/nanopb.options"
    TARGET
    machinetalk_proto_cc
    PROTOC_WORKDIR
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    PROTOS
    ${SOURCE_PROTO_FILES})

  protobuf_generate(
    LANGUAGE
    nanopb
    PROTOC_PLUGIN
    "--plugin=protoc-gen-nanopb=${nanopb_SOURCE_DIR}/generator/protoc-gen-nanopb"
    PROTOC_ARGUMENTS
    "--nanopb_opt=--generated-include-format='#include \"%s\"' --extension=.npb --options-file=${CMAKE_CURRENT_SOURCE_DIR}/src/nanopb.options"
    PROTOC_OUT_DIR
    "${PROTO_GENERATED_BASE_DIR}/src"
    IMPORT_DIRS
    "${nanopb_SOURCE_DIR}/generator/proto" # "${CMAKE_CURRENT_SOURCE_DIR}/src"
    GENERATE_EXTENSIONS
    ".npb.c"
    ".npb.h"
    PROTOC_WORKDIR
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    TARGET
    machinetalk_proto_c
    PROTOS
    ${SOURCE_PROTO_FILES})

  set_target_properties(
    machinetalk_proto_c
    PROPERTIES SOVERSION 0
               VERSION ${CMAKE_PROJECT_VERSION}
               POSITION_INDEPENDENT_CODE TRUE)

  set_target_properties(
    machinetalk_proto_cc
    PROPERTIES SOVERSION 0
               VERSION ${CMAKE_PROJECT_VERSION}
               POSITION_INDEPENDENT_CODE TRUE)
  install(
    TARGETS machinetalk_proto_c machinetalk_proto_cc
    EXPORT machinekit_hal
    LIBRARY DESTINATION "${MACHINEKIT_HAL_LIBRARY_DIRECTORY}"
            COMPONENT MachinekitHAL_Library_Base
            NAMELINK_COMPONENT MachinekitHAL_Library_Development
    PUBLIC_HEADER DESTINATION "${MACHINEKIT_HAL_INTERFACE_DIRECTORY}"
                  COMPONENT MachinekitHAL_Library_Development)
endif()
