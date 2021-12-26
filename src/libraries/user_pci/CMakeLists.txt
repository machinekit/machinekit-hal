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

option(BUILD_USER_PCI_LIBRARY "Built the User PCI library." TRUE)
if(BUILD_USER_PCI_LIBRARY)
  add_library(user_pci SHARED)

  set(SOURCE_FILES
      ${CMAKE_CURRENT_SOURCE_DIR}/src/device.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/firmware.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/string.c)

  set(PUBLIC_INCLUDE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include")

  set(PUBLIC_HEADER_FILES_BUILD
      ${PUBLIC_INCLUDE_DIRECTORY}/user_pci/config_module.h
      ${PUBLIC_INCLUDE_DIRECTORY}/user_pci/device.h
      ${PUBLIC_INCLUDE_DIRECTORY}/user_pci/firmware.h
      ${PUBLIC_INCLUDE_DIRECTORY}/user_pci/gfp.h
      ${PUBLIC_INCLUDE_DIRECTORY}/user_pci/list.h
      ${PUBLIC_INCLUDE_DIRECTORY}/user_pci/module.h
      ${PUBLIC_INCLUDE_DIRECTORY}/user_pci/slab.h
      ${PUBLIC_INCLUDE_DIRECTORY}/user_pci/string.h)

  set(PUBLIC_HEADER_FILES_INSTALL
      ${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/user_pci/config_module.h
      ${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/user_pci/device.h
      ${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/user_pci/firmware.h
      ${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/user_pci/gfp.h
      ${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/user_pci/list.h
      ${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/user_pci/module.h
      ${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/user_pci/slab.h
      ${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/user_pci/string.h)

  target_sources(
    user_pci
    PUBLIC "$<BUILD_INTERFACE:${PUBLIC_HEADER_FILES_BUILD}>"
           "$<INSTALL_INTERFACE:${PUBLIC_HEADER_FILES_INSTALL}>"
    PRIVATE ${SOURCE_FILES})

  target_include_directories(
    user_pci PUBLIC $<BUILD_INTERFACE:${PUBLIC_INCLUDE_DIRECTORY}>
                    $<INSTALL_INTERFACE:${MACHINEKIT_HAL_INTERFACE_DIRECTORY}>)

  target_link_libraries(user_pci PRIVATE managed_runtime managed_hal)

  set_target_properties(
    user_pci
    PROPERTIES SOVERSION 0
               PREFIX "libmkh"
               VERSION ${CMAKE_PROJECT_VERSION}
               PUBLIC_HEADER "${PUBLIC_HEADER_FILES_BUILD}"
               POSITION_INDEPENDENT_CODE TRUE
               PUBLIC_INCLUDE_DIRECTORY "${PUBLIC_INCLUDE_DIRECTORY}")

  install(
    TARGETS user_pci
    EXPORT machinekit_hal_user_pci
    LIBRARY DESTINATION "${MACHINEKIT_HAL_LIBRARIES_DIRECTORY}"
            COMPONENT MachinekitHAL_Library_User_PCI_Libraries
            NAMELINK_COMPONENT MachinekitHAL_Library_User_PCI_Namelinks
    PUBLIC_HEADER DESTINATION "${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/user_pci"
                  COMPONENT MachinekitHAL_Library_User_PCI_Headers)

  install(
    EXPORT machinekit_hal_user_pci
    DESTINATION ${MACHINEKIT_HAL_CMAKE_PACKAGE_DIRECTORY}
    NAMESPACE "${MACHINEKIT_HAL_NAMESPACE}::"
    FILE "MachinekitHALUserPCITarget.cmake"
    COMPONENT MachinekitHAL_Library_User_PCI_Package_Exports)

  export(
    EXPORT machinekit_hal_user_pci
    NAMESPACE "${MACHINEKIT_HAL_NAMESPACE}::"
    FILE "${MACHINEKIT_HAL_CMAKE_PACKAGE_OUTPUT_DIRECTORY}/MachinekitHALUserPCITarget.cmake"
  )

  cpack_add_component(MachinekitHAL_Library_User_PCI_Libraries
                      GROUP MachinekitHAL_Library_User_PCI)
  cpack_add_component(
    MachinekitHAL_Library_User_PCI_Namelinks
    GROUP MachinekitHAL_Library_User_PCI_Development
    DEPENDS MachinekitHAL_Library_User_PCI_Libraries)
  cpack_add_component(
    MachinekitHAL_Library_User_PCI_Headers
    GROUP MachinekitHAL_Library_User_PCI_Development
    DEPENDS MachinekitHAL_Library_User_PCI_Libraries)
  cpack_add_component(
    MachinekitHAL_Library_User_PCI_Package_Exports
    GROUP MachinekitHAL_Library_User_PCI_Development
    DEPENDS MachinekitHAL_Library_User_PCI_Libraries
            MachinekitHAL_Library_User_PCI_Headers
            MachinekitHAL_Library_User_PCI_Namelinks)

  # Specification of artifacts placement in package tree
  cpack_add_component_group(MachinekitHAL_Library_User_PCI
                            PARENT_GROUP MachinekitHAL_Package_Base_Libraries)
  cpack_add_component_group(
    MachinekitHAL_Library_User_PCI_Development
    PARENT_GROUP MachinekitHAL_Package_Base_Libraries_Development)
endif()
