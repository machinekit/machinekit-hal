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

if(NOT DEFINED SYS_IO_AVAILABLE)
  check_include_files("sys/io.h" SYS_IO_AVAILABLE)
endif()

if(SYS_IO_AVAILABLE)
  option(BUILD_UPCI_LIBRARY "Built the UPCI library." TRUE)

  if(BUILD_UPCI_LIBRARY)
    add_library(upci STATIC)

    set(SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/upci.c
                     ${CMAKE_CURRENT_SOURCE_DIR}/src/bitfile.c)

    set(PUBLIC_INCLUDE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include")

    set(PUBLIC_HEADER_FILES_BUILD ${PUBLIC_INCLUDE_DIRECTORY}/upci/bitfile.h
                                  ${PUBLIC_INCLUDE_DIRECTORY}/upci/upci.h)

    set(PUBLIC_HEADER_FILES_INSTALL
        ${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/upci/bitfile.h
        ${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/upci/upci.h)

    target_sources(
      upci
      PUBLIC "$<BUILD_INTERFACE:${PUBLIC_HEADER_FILES_BUILD}>"
             "$<INSTALL_INTERFACE:${PUBLIC_HEADER_FILES_INSTALL}>"
      PRIVATE ${SOURCE_FILES})

    target_include_directories(
      upci PUBLIC $<BUILD_INTERFACE:${PUBLIC_INCLUDE_DIRECTORY}>
                  $<INSTALL_INTERFACE:${MACHINEKIT_HAL_INTERFACE_DIRECTORY}>)

    set_target_properties(
      upci
      PROPERTIES PREFIX "libmkh"
                 PUBLIC_HEADER "${PUBLIC_HEADER_FILES_BUILD}"
                 POSITION_INDEPENDENT_CODE TRUE
                 PUBLIC_INCLUDE_DIRECTORY "${PUBLIC_INCLUDE_DIRECTORY}")

    install(
      TARGETS upci
      EXPORT machinekit_hal_upci
      ARCHIVE DESTINATION "${MACHINEKIT_HAL_LIBRARY_DIRECTORY}"
              COMPONENT MachinekitHAL_Library_UPCI_Archives
      PUBLIC_HEADER DESTINATION "${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/upci"
                    COMPONENT MachinekitHAL_Library_UPCI_Headers)

    install(
      EXPORT machinekit_hal_upci
      DESTINATION ${MACHINEKIT_HAL_CMAKE_PACKAGE_DIRECTORY}
      NAMESPACE "${MACHINEKIT_HAL_NAMESPACE}::"
      FILE "MachinekitHALUPCITarget.cmake"
      COMPONENT MachinekitHAL_Library_UPCI_Package_Exports)

    export(
      EXPORT machinekit_hal_upci
      NAMESPACE "${MACHINEKIT_HAL_NAMESPACE}::"
      FILE "${MACHINEKIT_HAL_CMAKE_PACKAGE_OUTPUT_DIRECTORY}/MachinekitHALUPCITarget.cmake"
    )

    cpack_add_component(MachinekitHAL_Library_UPCI_Archives
                        GROUP MachinekitHAL_Library_UPCI_Development)
    cpack_add_component(
      MachinekitHAL_Library_UPCI_Headers
      GROUP MachinekitHAL_Library_UPCI_Development
      DEPENDS MachinekitHAL_Library_UPCI_Archives)
    cpack_add_component(
      MachinekitHAL_Library_UPCI_Package_Exports
      GROUP MachinekitHAL_Library_UPCI_Development
      DEPENDS MachinekitHAL_Library_UPCI_Archives
              MachinekitHAL_Library_UPCI_Headers)

    # Specification of artifacts placement in package tree
    cpack_add_component_group(
      MachinekitHAL_Library_UPCI_Development
      PARENT_GROUP MachinekitHAL_Package_Base_Libraries_Development)
  endif()
endif()
