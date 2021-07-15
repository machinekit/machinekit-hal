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

  set(PUBLIC_HEADER_FILES
      ${CMAKE_CURRENT_SOURCE_DIR}/include/user_pci/config_module.h
      ${CMAKE_CURRENT_SOURCE_DIR}/include/user_pci/device.h
      ${CMAKE_CURRENT_SOURCE_DIR}/include/user_pci/firmware.h
      ${CMAKE_CURRENT_SOURCE_DIR}/include/user_pci/gfp.h
      ${CMAKE_CURRENT_SOURCE_DIR}/include/user_pci/list.h
      ${CMAKE_CURRENT_SOURCE_DIR}/include/user_pci/module.h
      ${CMAKE_CURRENT_SOURCE_DIR}/include/user_pci/slab.h
      ${CMAKE_CURRENT_SOURCE_DIR}/include/user_pci/string.h)

  target_sources(
    user_pci
    PUBLIC ${PUBLIC_HEADER_FILES}
    PRIVATE ${SOURCE_FILES})

  target_include_directories(
    user_pci PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                    $<INSTALL_INTERFACE:${MACHINEKIT_HAL_INTERFACE_DIRECTORY}>)

  target_link_libraries(user_pci PRIVATE runtime_api hal_api)

  set_target_properties(
    user_pci
    PROPERTIES SOVERSION 0
               PREFIX "libmkh"
               VERSION ${CMAKE_PROJECT_VERSION}
               PUBLIC_HEADER "${PUBLIC_HEADER_FILES}"
               POSITION_INDEPENDENT_CODE TRUE)

  target_compile_definitions(user_pci PRIVATE "RTAPI")

  install(
    TARGETS user_pci
    EXPORT machinekit_hal_user_pci
    LIBRARY DESTINATION "${MACHINEKIT_HAL_LIBRARIES_DIRECTORY}"
            COMPONENT MachinekitHAL_Library_User_PCI_Libraries
    PUBLIC_HEADER DESTINATION "${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/user_pci"
                  COMPONENT MachinekitHAL_Library_User_PCI_Headers)

  cpack_add_component(MachinekitHAL_Library_User_PCI_Libraries
                      GROUP MachinekitHAL_Library_User_PCI)
  cpack_add_component(
    MachinekitHAL_Library_User_PCI_Headers
    GROUP MachinekitHAL_Library_User_PCI_Development
    DEPENDS MachinekitHAL_Library_User_PCI_Libraries)

  # Specification of artifacts placement in package tree
  cpack_add_component_group(MachinekitHAL_Library_User_PCI
                            PARENT_GROUP MachinekitHAL_Package_Base_Libraries)
  cpack_add_component_group(
    MachinekitHAL_Library_User_PCI_Development
    PARENT_GROUP MachinekitHAL_Package_Base_Libraries_Development)
endif()
