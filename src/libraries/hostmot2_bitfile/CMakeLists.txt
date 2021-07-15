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

option(BUILD_HOSTMOT2_BITFILE_LIBRARY "Define the Hostmot2 Bitfile library."
       TRUE)

if(BUILD_HOSTMOT2_BITFILE_LIBRARY)
  add_library(hostmot2_bitfile STATIC)
  add_library(${MACHINEKIT_HAL_NAMESPACE}::hostmot2_bitfile ALIAS
              hostmot2_bitfile)

  set(SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/bitfile.c)
  set(PUBLIC_HEADER_FILES
      ${CMAKE_CURRENT_SOURCE_DIR}/include/hostmot2/bitfile.h)

  target_sources(
    hostmot2_bitfile
    PUBLIC ${PUBLIC_HEADER_FILES}
    PRIVATE ${SOURCE_FILES})

  target_include_directories(
    hostmot2_bitfile
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
           $<INSTALL_INTERFACE:${MACHINEKIT_HAL_INTERFACE_DIRECTORY}>)

  target_link_libraries(
    hostmot2_bitfile
    PUBLIC user_pci
    PRIVATE runtime_api hal_api runtime_math hostmot2_api)

  target_compile_definitions(hostmot2_bitfile PRIVATE "RTAPI")

  set_target_properties(
    hostmot2_bitfile PROPERTIES PREFIX "libmkh" PUBLIC_HEADER
                                                "${PUBLIC_HEADER_FILES}")

  install(
    TARGETS hostmot2_bitfile
    EXPORT machinekit_hal_hostmot2_bitfile
    ARCHIVE DESTINATION "${MACHINEKIT_HAL_LIBRARY_DIRECTORY}"
            COMPONENT MachinekitHAL_Library_Hostmot_Bitfile_Libraries
    PUBLIC_HEADER DESTINATION "${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/hostmot2"
                  COMPONENT MachinekitHAL_Library_Hostmot_Bitfile_Headers)

  cpack_add_component(
    MachinekitHAL_Library_Hostmot_Bitfile_Libraries
    GROUP MachinekitHAL_Library_Hostmot_Bitfile_Development
    DEPENDS MachinekitHAL_Library_Runtime_Math_Libraries)
  cpack_add_component(MachinekitHAL_Library_Hostmot_Bitfile_Headers
                      GROUP MachinekitHAL_Library_Hostmot_Bitfile_Development)

  # Specification of artifacts placement in package tree
  cpack_add_component_group(
    MachinekitHAL_Library_Hostmot_Bitfile_Development
    PARENT_GROUP MachinekitHAL_Package_Base_Libraries_Development)
endif()
