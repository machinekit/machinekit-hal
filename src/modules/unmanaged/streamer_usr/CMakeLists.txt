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

option(BUILD_STREAMER_USR_UNMANAGED_MODULE
       "Build the Streamer Usr unmanaged module." TRUE)

if(BUILD_STREAMER_USR_UNMANAGED_MODULE)
  add_executable(streamer_usr)

  set(SOURCE_FILES src/streamer_usr.c)

  target_sources(streamer_usr PRIVATE ${SOURCE_FILES})

  target_link_libraries(streamer_usr PRIVATE hal samstr_memdefs)

  set_target_properties(streamer_usr PROPERTIES OUTPUT_NAME "halstreamer")
endif()
