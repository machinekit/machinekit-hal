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

if(CMAKE_SYSTEM_PROCESSOR STREQUAL "arm")
  option(BUILD_PRU_APP_LOADER_LIBRARY "Built the PRU App Loader library." TRUE)

  if(BUILD_PRU_APP_LOADER_LIBRARY)
    add_library(pru_app_loader SHARED)
    add_library(${MACHINEKIT_HAL_NAMESPACE}::pru_app_loader ALIAS
                pru_app_loader)

    set(SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/prussdrv.c)

    set(PUBLIC_INCLUDE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include")

    set(PUBLIC_HEADER_FILES
        ${PUBLIC_INCLUDE_DIRECTORY}/pru_app_loader/__prussdrv.h
        ${PUBLIC_INCLUDE_DIRECTORY}/pru_app_loader/pruss_intc_mapping.h
        ${PUBLIC_INCLUDE_DIRECTORY}/pru_app_loader/prussdrv.h)

    target_sources(pru_app_loader PRIVATE ${SOURCE_FILES}
                                          ${PUBLIC_HEADER_FILES})

    target_include_directories(
      pru_app_loader
      PUBLIC $<BUILD_INTERFACE:${PUBLIC_INCLUDE_DIRECTORY}>
             $<INSTALL_INTERFACE:${MACHINEKIT_HAL_INTERFACE_DIRECTORY}>)

    target_link_libraries(pru_app_loader PRIVATE managed_runtime runtime_config)

    set_target_properties(
      pru_app_loader
      PROPERTIES SOVERSION 0
                 PREFIX "libmkh"
                 VERSION ${CMAKE_PROJECT_VERSION}
                 PUBLIC_HEADER "${PUBLIC_HEADER_FILES}"
                 POSITION_INDEPENDENT_CODE TRUE
                 PUBLIC_INCLUDE_DIRECTORY "${PUBLIC_INCLUDE_DIRECTORY}")
    install(
      TARGETS pru_app_loader
      EXPORT machinekit_hal_pru_app_loader
      LIBRARY DESTINATION "${MACHINEKIT_HAL_LIBRARY_DIRECTORY}"
              COMPONENT MachinekitHAL_Library_PRU_App_Loader_Libraries
              NAMELINK_COMPONENT MachinekitHAL_Library_PRU_App_Loader_Namelinks
      PUBLIC_HEADER
        DESTINATION "${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/pru_app_loader"
        COMPONENT MachinekitHAL_Library_PRU_App_Loader_Headers)

    install(
      EXPORT machinekit_hal_pru_app_loader
      DESTINATION ${MACHINEKIT_HAL_CMAKE_PACKAGE_DIRECTORY}
      NAMESPACE "${MACHINEKIT_HAL_NAMESPACE}::"
      FILE "MachinekitHALPRUAppLoaderTarget.cmake"
      COMPONENT MachinekitHAL_Library_PRU_App_Loader_Package_Exports)

    export(
      EXPORT machinekit_hal_pru_app_loader
      NAMESPACE "${MACHINEKIT_HAL_NAMESPACE}::"
      FILE "${MACHINEKIT_HAL_CMAKE_PACKAGE_OUTPUT_DIRECTORY}/MachinekitHALPRUAppLoaderTarget.cmake"
    )

    cpack_add_component(MachinekitHAL_Library_PRU_App_Loader_Libraries
                        GROUP MachinekitHAL_Library_PRU_App_Loader)
    cpack_add_component(
      MachinekitHAL_Library_PRU_App_Loader_Namelinks
      GROUP MachinekitHAL_Library_PRU_App_Loader_Development
      DEPENDS MachinekitHAL_Library_PRU_App_Loader_Libraries)
    cpack_add_component(
      MachinekitHAL_Library_PRU_App_Loader_Headers
      GROUP MachinekitHAL_Library_PRU_App_Loader_Development
      DEPENDS MachinekitHAL_Library_PRU_App_Loader_Libraries)
    cpack_add_component(
      MachinekitHAL_Library_PRU_App_Loader_Package_Exports
      GROUP MachinekitHAL_Library_PRU_App_Loader_Development
      DEPENDS MachinekitHAL_Library_PRU_App_Loader_Libraries
              MachinekitHAL_Library_PRU_App_Loader_Headers
              MachinekitHAL_Library_PRU_App_Loader_Namelinks)

    # Specification of artifacts placement in package tree
    cpack_add_component_group(MachinekitHAL_Library_PRU_App_Loader
                              PARENT_GROUP MachinekitHAL_Package_Base_Libraries)
    cpack_add_component_group(
      MachinekitHAL_Library_PRU_App_Loader_Development
      PARENT_GROUP MachinekitHAL_Package_Base_Libraries_Development)
  endif()
endif()
