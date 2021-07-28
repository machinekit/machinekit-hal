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

option(BUILD_GS2_VFD_UNMANAGED_MODULE "Build the gs2 VFD unmanaged module."
       TRUE)

if(BUILD_GS2_VFD_UNMANAGED_MODULE)
  add_executable(gs2_vfd)

  set(SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/gs2_vfd.c)

  target_sources(gs2_vfd PRIVATE ${SOURCE_FILES})

  pkg_check_modules(LIBMODBUS "libmodbus" REQUIRED IMPORTED_TARGET)

  target_link_libraries(gs2_vfd PRIVATE runtime hal PkgConfig::LIBMODBUS)

  install(TARGETS gs2_vfd
          RUNTIME DESTINATION "${MACHINEKIT_HAL_UNMANAGED_MODULE_DIRECTORY}"
                  COMPONENT MachinekitHAL_Unmanaged_Module_GS2_VFD_Drivers)

  cpack_add_component(
    MachinekitHAL_Unmanaged_Module_GS2_VFD_Drivers
    GROUP MachinekitHAL_Unmanaged_Module_GS2_VFD
    DEPENDS MachinekitHAL_Library_Runtime_Libraries
            MachinekitHAL_Library_HAL_Libraries)

  # Specification of artifacts placement in package tree
  cpack_add_component_group(
    MachinekitHAL_Unmanaged_Module_GS2_VFD
    PARENT_GROUP MachinekitHAL_Package_Base_Unmanaged_Modules_Drivers)
endif()
