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

option(BUILD_HALMETER_UNMANAGED_MODULE "Build the HALmeter unmanaged module."
       TRUE)

if(BUILD_HALMETER_UNMANAGED_MODULE)
  add_executable(halmeter)

  set(SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/meter.c)

  target_sources(halmeter PRIVATE ${SOURCE_FILES})

  target_link_libraries(halmeter PRIVATE runtime hal miscgtk)

  # TODO: Replace the placeholder
  target_compile_definitions(
    halmeter PRIVATE "HAL_PO_DIR=\"${CMAKE_CURRENT_BINARY_FIR}/share/locale\"")

  install(TARGETS halmeter
          RUNTIME DESTINATION "${MACHINEKIT_HAL_UNMANAGED_MODULE_DIRECTORY}"
                  COMPONENT MachinekitHAL_Unmanaged_Module_Halmeter_Components)

  cpack_add_component(
    MachinekitHAL_Unmanaged_Module_Halmeter_Components
    GROUP MachinekitHAL_Unmanaged_Module_Halmeter
    DEPENDS MachinekitHAL_Library_Runtime_Libraries
            MachinekitHAL_Library_HAL_Libraries
            MachinekitHAL_Library_Misc_GTK_Libraries)

  # Specification of artifacts placement in package tree
  cpack_add_component_group(
    MachinekitHAL_Unmanaged_Module_Halmeter
    PARENT_GROUP MachinekitHAL_Package_Base_Unmanaged_Modules_Components)
endif()
