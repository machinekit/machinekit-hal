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

set(NANOPB_UPSTREAM_REPOSITORY
    "https://github.com/cerna/nanopb.git"
    CACHE STRING "Git address of upstream Nanopb repository.")
set(NANOPB_CHECKOUT_REF
    "master"
    CACHE STRING "Ref of upstream Nanopb repository to check out.")

include(FetchContent)
include("${PROJECT_SOURCE_DIR}/support/cmake/tools/FindProtobuf.cmake")

# Both FETCHCONTENT_QUIET and GIT_PROGRESS need to be set to show progress
# status during configuring state
set(FETCHCONTENT_QUIET FALSE)

FetchContent_Declare(
  nanopb
  GIT_REPOSITORY "${NANOPB_UPSTREAM_REPOSITORY}"
  GIT_TAG "${NANOPB_CHECKOUT_REF}"
  GIT_PROGRESS TRUE
  GIT_REMOTE_UPDATE_STRATEGY "REBASE_CHECKOUT" PREFIX
  "${CMAKE_CURRENT_BINARY_DIR}/nanopb" CMAKE_ARGS
  -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>)

# Set the DEFAULT COMPONENT name for third party Nanopb project
set(CMAKE_INSTALL_DEFAULT_COMPONENT_NAME "Nanopb")
FetchContent_MakeAvailable(nanopb)

# Makes a linkage (dependency) on the base file, which means the build command
# will not rebuild the whole *.proto libraries on each run but only on a change
configure_file(${nanopb_SOURCE_DIR}/generator/proto/nanopb.proto
               ${CMAKE_CURRENT_BINARY_DIR}/nanopb.proto COPYONLY)

set(SOURCE_NANOPB_PROTO_FILE ${CMAKE_CURRENT_BINARY_DIR}/nanopb.proto)

# As upstream CMake implementation does not implement usable CMake targets
# usable to just target_link_libraries() against, create internal ones

add_library(nanopb_proto_cc STATIC)
add_library(nanopb_interface INTERFACE)

target_include_directories(
  nanopb_proto_cc
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
         $<INSTALL_INTERFACE:${MACHINEKIT_HAL_INTERFACE_DIRECTORY}>)

protobuf_generate(
  LANGUAGE
  cpp
  PROTOC_OUT_DIR
  "${CMAKE_CURRENT_BINARY_DIR}"
  IMPORT_DIRS
  "${CMAKE_CURRENT_BINARY_DIR}"
  TARGET
  nanopb_proto_cc
  PROTOS
  ${SOURCE_NANOPB_PROTO_FILE}
  APPEND_PATH)

target_include_directories(
  nanopb_interface
  INTERFACE $<BUILD_INTERFACE:${nanopb_SOURCE_DIR}>
            $<INSTALL_INTERFACE:${MACHINEKIT_HAL_INTERFACE_DIRECTORY}>)

# TODO: Add install public headers step?

set_target_properties(nanopb_proto_cc PROPERTIES POSITION_INDEPENDENT_CODE TRUE)
set_target_properties(protobuf-nanopb-static
                      PROPERTIES POSITION_INDEPENDENT_CODE TRUE)

target_link_libraries(nanopb_interface INTERFACE protobuf-nanopb-static)

install(
  TARGETS nanopb_interface protobuf-nanopb-static
  EXPORT machinekit_hal_nanopb_interface
  ARCHIVE DESTINATION "${MACHINEKIT_HAL_LIBRARY_DIRECTORY}"
          COMPONENT MachinekitHAL_Library_NanoPB_Archives
  PUBLIC_HEADER DESTINATION "${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/upci"
                COMPONENT MachinekitHAL_Library_NanoPB_Headers)

install(
  TARGETS nanopb_proto_cc
  EXPORT machinekit_hal_nanopb_proto_cc
  ARCHIVE DESTINATION "${MACHINEKIT_HAL_LIBRARY_DIRECTORY}"
          COMPONENT MachinekitHAL_Library_NanoPB_Archives
  PUBLIC_HEADER DESTINATION "${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/upci"
                COMPONENT MachinekitHAL_Library_NanoPB_Headers)

#[[install(
  TARGETS protobuf-nanopb-static
  EXPORT machinekit_hal_protobuf_nanopb_static
  ARCHIVE DESTINATION "${MACHINEKIT_HAL_LIBRARY_DIRECTORY}"
          COMPONENT MachinekitHAL_Library_NanoPB_Archives
  PUBLIC_HEADER DESTINATION "${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/upci"
                COMPONENT MachinekitHAL_Library_NanoPB_Headers)]]

install(
  EXPORT machinekit_hal_nanopb_interface
  DESTINATION ${MACHINEKIT_HAL_CMAKE_PACKAGE_DIRECTORY}
  NAMESPACE "${MACHINEKIT_HAL_NAMESPACE}::"
  FILE "MachinekitHALNanoPBInterfaceTarget.cmake"
  COMPONENT MachinekitHAL_Library_NanoPB_Package_Exports)

install(
  EXPORT machinekit_hal_nanopb_proto_cc
  DESTINATION ${MACHINEKIT_HAL_CMAKE_PACKAGE_DIRECTORY}
  NAMESPACE "${MACHINEKIT_HAL_NAMESPACE}::"
  FILE "MachinekitHALNanoPBProtoCCTarget.cmake"
  COMPONENT MachinekitHAL_Library_NanoPB_Package_Exports)

#[[install(
  EXPORT machinekit_hal_protobuf_nanopb_static
  DESTINATION ${MACHINEKIT_HAL_CMAKE_PACKAGE_DIRECTORY}
  NAMESPACE "${MACHINEKIT_HAL_NAMESPACE}::"
  FILE "MachinekitHALProtobufNanoPBStaticTarget.cmake"
  COMPONENT MachinekitHAL_Library_NanoPB_Package_Exports)]]

export(
  EXPORT machinekit_hal_nanopb_interface
  NAMESPACE "${MACHINEKIT_HAL_NAMESPACE}::"
  FILE "${MACHINEKIT_HAL_CMAKE_PACKAGE_OUTPUT_DIRECTORY}/MachinekitHALNanoPBInterfaceTarget.cmake"
)

export(
  EXPORT machinekit_hal_nanopb_proto_cc
  NAMESPACE "${MACHINEKIT_HAL_NAMESPACE}::"
  FILE "${MACHINEKIT_HAL_CMAKE_PACKAGE_OUTPUT_DIRECTORY}/MachinekitHALNanoPBProtoCCTarget.cmake"
)

#[[export(
  TARGETS protobuf-nanopb-static
  NAMESPACE "${MACHINEKIT_HAL_NAMESPACE}::"
  FILE "${MACHINEKIT_HAL_CMAKE_PACKAGE_OUTPUT_DIRECTORY}/MachinekitHALProtobufNanoPBStaticTarget.cmake"
)]]

cpack_add_component(MachinekitHAL_Library_NanoPB_Archives
                    GROUP MachinekitHAL_Library_NanoPB_Development)
cpack_add_component(MachinekitHAL_Library_NanoPB_Headers
                    GROUP MachinekitHAL_Library_NanoPB_Development)
cpack_add_component(
  MachinekitHAL_Library_NanoPB_Package_Exports
  GROUP MachinekitHAL_Library_NanoPB_Development
  DEPENDS MachinekitHAL_Library_NanoPB_Archives
          MachinekitHAL_Library_NanoPB_Headers)

# Specification of artifacts placement in package tree
cpack_add_component_group(
  MachinekitHAL_Library_NanoPB_Development
  PARENT_GROUP MachinekitHAL_Package_Base_Libraries_Development)
