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

option(BUILD_CONSTV2_UNMANAGED_MODULE "Build the const/v2 unmanaged module."
       TRUE)

if(BUILD_CONSTV2_UNMANAGED_MODULE)
  add_executable(const)
  add_executable(constv2)

  set(CONST_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/const.cc)
  set(CONSTV2_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/constv2.cc)
  set(PRIVATE_HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/LCNC_Headers.h)

  target_sources(const PRIVATE ${CONST_SOURCE_FILES} ${PRIVATE_HEADER_FILES})
  target_sources(constv2 PRIVATE ${CONSTV2_SOURCE_FILES}
                                 ${PRIVATE_HEADER_FILES})

  find_library(MATH NAMES "m" REQUIRED)

  target_link_libraries(const PRIVATE runtime hal mkini ${MATH})
  target_link_libraries(constv2 PRIVATE runtime hal mkini ${MATH})

  install(TARGETS const constv2
          RUNTIME DESTINATION "${MACHINEKIT_HAL_UNMANAGED_MODULE_DIRECTORY}"
                  COMPONENT MachinekitHAL_Unmanaged_Module_Const_Components)

  cpack_add_component(
    MachinekitHAL_Unmanaged_Module_Const_Components
    GROUP MachinekitHAL_Unmanaged_Module_Const
    DEPENDS MachinekitHAL_Library_Runtime_Libraries
            MachinekitHAL_Library_HAL_Libraries
            MachinekitHAL_Library_MKIni_Libraries)

  # Specification of artifacts placement in package tree
  cpack_add_component_group(
    MachinekitHAL_Unmanaged_Module_Const
    PARENT_GROUP MachinekitHAL_Package_Base_Unmanaged_Modules_Components)
endif()
