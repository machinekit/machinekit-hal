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

option(BUILD_XHC_WBH04B_6_UNMANAGED_MODULE
       "Build the XHC WBH04B-6 unmanaged module." TRUE)

if(BUILD_XHC_WBH04B_6_UNMANAGED_MODULE)
  add_executable(xhc_whb04b_6)

  set(SOURCE_FILES
      ${CMAKE_CURRENT_SOURCE_DIR}/src/hal.cc
      ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cc
      ${CMAKE_CURRENT_SOURCE_DIR}/src/pendant-types.cc
      ${CMAKE_CURRENT_SOURCE_DIR}/src/pendant.cc
      ${CMAKE_CURRENT_SOURCE_DIR}/src/usb.cc
      ${CMAKE_CURRENT_SOURCE_DIR}/src/xhc-whb04b6.cc)

  set(PRIVATE_HEADER_FILES
      ${CMAKE_CURRENT_SOURCE_DIR}/src/hal.h
      ${CMAKE_CURRENT_SOURCE_DIR}/src/pendant-types.h
      ${CMAKE_CURRENT_SOURCE_DIR}/src/pendant.h
      ${CMAKE_CURRENT_SOURCE_DIR}/src/usb.h
      ${CMAKE_CURRENT_SOURCE_DIR}/src/xhc-whb04b6.h)

  target_sources(xhc_whb04b_6 PRIVATE ${SOURCE_FILES} ${PRIVATE_HEADER_FILES})

  pkg_check_modules(LIBUSB "libusb-1.0" REQUIRED IMPORTED_TARGET)

  target_link_libraries(xhc_whb04b_6 PRIVATE hal mkini machinetalk runtime_math
                                             PkgConfig::LIBUSB)

  # TODO: Replace the placeholder
  target_compile_definitions(xhc_whb04b_6 PRIVATE "PACKAGE_VERSION=00000000")

  install(TARGETS xhc_hb04
          RUNTIME DESTINATION "${MACHINEKIT_HAL_UNMANAGED_MODULE_DIRECTORY}"
                  COMPONENT MachinekitHAL_Unmanaged_Module_XHC_HB04_Drivers)

  cpack_add_component(
    MachinekitHAL_Unmanaged_Module_XHC_HB04_Drivers
    GROUP MachinekitHAL_Unmanaged_Module_XHC_HB04
    DEPENDS MachinekitHAL_Library_HAL_Libraries
            MachinekitHAL_Library_MKIni_Libraries
            MachinekitHAL_Library_Runtime_Math_Libraries)

  # Specification of artifacts placement in package tree
  cpack_add_component_group(
    MachinekitHAL_Unmanaged_Module_XHC_HB04
    PARENT_GROUP MachinekitHAL_Package_Base_Unmanaged_Modules_Drivers)
endif()
