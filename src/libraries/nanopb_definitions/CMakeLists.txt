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

option(BUILD_NANOPB_DEFINITIONS_LIBRARY "Built the NanoPB definitions library."
       TRUE)

if(BUILD_NANOPB_DEFINITIONS_LIBRARY)
  add_library(managed_nanopb_definitions SHARED)
  add_library(${MACHINEKIT_HAL_NAMESPACE}::managed_nanopb_definitions ALIAS
              managed_nanopb_definitions)

  set(SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/nanopb.c)

  target_sources(managed_nanopb_definitions PRIVATE ${SOURCE_FILES})

  target_link_libraries(managed_nanopb_definitions
                        PRIVATE nanopb_interface managed_runtime machinetalk)

  target_compile_definitions(managed_nanopb_definitions
                             PRIVATE "PB_FIELD_32BIT")

  set_target_properties(
    managed_nanopb_definitions
    PROPERTIES SOVERSION 0
               PREFIX "libmkh"
               VERSION ${CMAKE_PROJECT_VERSION}
               POSITION_INDEPENDENT_CODE TRUE)

  install(
    TARGETS managed_nanopb_definitions
    EXPORT machinekit_hal_managed_nanopb_definitions
    LIBRARY
      DESTINATION "${MACHINEKIT_HAL_LIBRARY_DIRECTORY}"
      COMPONENT MachinekitHAL_Library_Managed_NanoPB_Definitions_Libraries
      NAMELINK_COMPONENT
        MachinekitHAL_Library_Managed_NanoPB_Definitions_Namelinks)

  install(
    EXPORT machinekit_hal_managed_nanopb_definitions
    DESTINATION ${MACHINEKIT_HAL_CMAKE_PACKAGE_DIRECTORY}
    NAMESPACE "${MACHINEKIT_HAL_NAMESPACE}::"
    FILE "MachinekitHALNanoPBDefinitionsTarget.cmake"
    COMPONENT MachinekitHAL_Library_Managed_NanoPB_Definitions_Package_Exports)

  export(
    EXPORT machinekit_hal_managed_nanopb_definitions
    NAMESPACE "${MACHINEKIT_HAL_NAMESPACE}::"
    FILE "${MACHINEKIT_HAL_CMAKE_PACKAGE_OUTPUT_DIRECTORY}/MachinekitHALNanoPBDefinitionsTarget.cmake"
  )

  cpack_add_component(MachinekitHAL_Library_Managed_NanoPB_Definitions_Libraries
                      GROUP MachinekitHAL_Library_Managed_NanoPB_Definitions)
  cpack_add_component(
    MachinekitHAL_Library_Managed_NanoPB_Definitions_Namelinks
    GROUP MachinekitHAL_Library_Managed_NanoPB_Definitions_Development
    DEPENDS MachinekitHAL_Library_Managed_NanoPB_Definitions_Libraries)
  cpack_add_component(
    MachinekitHAL_Library_Managed_NanoPB_Definitions_Package_Exports
    GROUP MachinekitHAL_Library_Managed_NanoPB_Definitions_Development
    DEPENDS MachinekitHAL_Library_Managed_NanoPB_Definitions_Libraries
            MachinekitHAL_Library_Managed_NanoPB_Definitions_Namelinks)

  # Specification of artifacts placement in package tree
  cpack_add_component_group(MachinekitHAL_Library_Managed_NanoPB_Definitions
                            PARENT_GROUP MachinekitHAL_Package_Base_Libraries)
  cpack_add_component_group(
    MachinekitHAL_Library_Managed_NanoPB_Definitions_Development
    PARENT_GROUP MachinekitHAL_Package_Base_Libraries_Development)
endif()
