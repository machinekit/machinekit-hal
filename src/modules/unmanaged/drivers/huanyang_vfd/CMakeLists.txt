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

option(BUILD_HUANYANG_VFD_UNMANAGED_MODULE
       "Build the Huanyang VFD unmanaged module." TRUE)

if(BUILD_HUANYANG_VFD_UNMANAGED_MODULE)
  add_executable(huanyang_vfd)

  set(SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/hy_comm.c
                   ${CMAKE_CURRENT_SOURCE_DIR}/src/hy_vfd.c)

  set(PRIVATE_HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/hy_comm.h)

  target_sources(huanyang_vfd PRIVATE ${SOURCE_FILES} ${PRIVATE_HEADER_FILES})

  target_link_libraries(huanyang_vfd PRIVATE hal ${CMAKE_LIBS_LD})

  install(
    TARGETS huanyang_vfd
    RUNTIME DESTINATION "${MACHINEKIT_HAL_UNMANAGED_MODULE_DIRECTORY}"
            COMPONENT MachinekitHAL_Unmanaged_Module_Huyanyang_VFD_Drivers)

  cpack_add_component(
    MachinekitHAL_Unmanaged_Module_Huyanyang_VFD_Drivers
    GROUP MachinekitHAL_Unmanaged_Module_Huyanyang_VFD
    DEPENDS MachinekitHAL_Library_HAL_Libraries)

  # Specification of artifacts placement in package tree
  cpack_add_component_group(
    MachinekitHAL_Unmanaged_Module_Huyanyang_VFD
    PARENT_GROUP MachinekitHAL_Package_Base_Unmanaged_Modules_Drivers)
endif()
