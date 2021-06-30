# ##############################################################################
# Description:  CMakeLists.txt
#
# This file, 'CMakeLists.txt', implements build system rules for Machinekit-HAL
# project
#
# Copyright (C) 2021    Jakub Fišer  <jakub DOT fiser AT eryaf DOT com>
#
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU Lesser General Public License as published by the Free
# Software Foundation; either version 2.1 of the License, or (at your option)
# any later version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
# details.
#
# You should have received a copy of the GNU Lesser General Public License along
# with this library; if not, write to the Free Software Foundation, Inc., 51
# Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
#
# ##############################################################################

option(BUILD_HALCMD_EXECUTABLE "Build the HALcmd executable." TRUE)

if(BUILD_HALCMD_EXECUTABLE)
  add_executable(halcmd)

  set(SOURCE_FILES src/halcmd_completion.c src/halcmd_main.c)
  set(PRIVATE_HEADER_FILES src/halcmd_completion.h)

  find_package(Readline REQUIRED)

  target_sources(halcmd PRIVATE ${SOURCE_FILES} ${PRIVATE_HEADER_FILES})

  target_compile_definitions(halcmd PRIVATE "ULAPI")

  pkg_check_modules(AVAHICLIENT "avahi-client" REQUIRED IMPORTED_TARGET)
  pkg_check_modules(PROTOBUF "protobuf" REQUIRED IMPORTED_TARGET)

  # TODO: Needed just because of the config.h??
  target_link_libraries(
    halcmd
    PRIVATE runtime
            hal
            hal_command
            mkini
            PkgConfig::AVAHICLIENT
            PkgConfig::PROTOBUF
            READLINE::READLINE)
endif()
