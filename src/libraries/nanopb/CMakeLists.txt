#####################################################################
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
######################################################################

set(NANOPB_UPSTREAM_REPOSITORY "https://github.com/nanopb/nanopb.git"
    CACHE STRING "Git address of upstream Nanopb repository.")
set(NANOPB_CHECKOUT_REF "master"
    CACHE STRING "Ref of upstream Nanopb repository to check out.")

include(FetchContent)
include("${PROJECT_SOURCE_DIR}/cmake/tools/FindProtobuf.cmake")

# Both FETCHCONTENT_QUIET and GIT_PROGRESS need to be set to show progress
# status during configuring state
set(FETCHCONTENT_QUIET FALSE)

FetchContent_Declare(nanopb
    GIT_REPOSITORY "${NANOPB_UPSTREAM_REPOSITORY}"
    GIT_TAG "${NANOPB_CHECKOUT_REF}"
    GIT_PROGRESS TRUE
    GIT_REMOTE_UPDATE_STRATEGY "REBASE_CHECKOUT"
    PREFIX "${CMAKE_CURRENT_BINARY_DIR}/nanopb"
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
)

FetchContent_MakeAvailable(nanopb)

# Makes a linkage (dependency) on the base file, which means the build command will not
# rebuild the whole *.proto libraries on each run but only on a change
configure_file(${nanopb_SOURCE_DIR}/generator/proto/nanopb.proto ${CMAKE_CURRENT_BINARY_DIR}/nanopb.proto COPYONLY)

set(SOURCE_NANOPB_PROTO_FILE ${CMAKE_CURRENT_BINARY_DIR}/nanopb.proto)

# As upstream CMake implementation does not implement usable
# CMake targets usable to just target_link_libraries() against,
# create internal ones

add_library(nanopb_proto_cc STATIC)
add_library(nanopb_interface INTERFACE)

target_include_directories(nanopb_proto_cc PUBLIC "${CMAKE_CURRENT_BINARY_DIR}")

protobuf_generate(
 LANGUAGE cpp
 PROTOC_OUT_DIR "${CMAKE_CURRENT_BINARY_DIR}"
 IMPORT_DIRS "${CMAKE_CURRENT_BINARY_DIR}"
 TARGET nanopb_proto_cc
 PROTOS ${SOURCE_NANOPB_PROTO_FILE})

target_include_directories(nanopb_interface INTERFACE ${nanopb_SOURCE_DIR}) # TODO: Add both install and build INTERFACEs

# TODO: Add install public headers step?

set_target_properties(
    nanopb_proto_cc
    PROPERTIES
        POSITION_INDEPENDENT_CODE TRUE)
