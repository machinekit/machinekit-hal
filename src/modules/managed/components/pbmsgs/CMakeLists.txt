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

# TODO: Full of errors, will need more thoughts
#[[
option(BUILD_PBMSGS_MANAGED_MODULE "Built the PBMsgs HAL managed module." TRUE)

if(BUILD_PBMSGS_MANAGED_MODULE)
  include(MachinekitHALSymbolVisibility)

  add_library(pbmsgs MODULE)
  add_library(${MACHINEKIT_HAL_NAMESPACE}::pbmsgs ALIAS pbmsgs)

  set(SOURCE_FILES src/pbmsgs.c)
  set(PUBLIC_HEADER_FILES include/pbmsgs.h)

  target_sources(pbmsgs PRIVATE ${SOURCE_FILES} ${PUBLIC_HEADER_FILES})

  target_include_directories(pbmsgs PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

  target_link_libraries(pbmsgs PRIVATE hal_api runtime_api machinetalk)

  export_rtapi_symbols(TARGET pbmsgs)

  target_compile_definitions(pbmsgs PRIVATE "RTAPI")

  set_target_properties(
    pbmsgs
    PROPERTIES PREFIX "mod" LIBRARY_OUTPUT_DIRECTORY
                            "${MACHINEKIT_HAL_MANAGED_MODULE_OUTPUT_DIRECTORY}")

  install(
    TARGETS pbmsgs
    EXPORT machinekit_hal_managed_module_pbmsgs
    LIBRARY DESTINATION "${MACHINEKIT_HAL_MANAGED_MODULE_DIRECTORY}"
            COMPONENT MachinekitHAL_Managed_Module_PBMsgs)
endif()
]]
