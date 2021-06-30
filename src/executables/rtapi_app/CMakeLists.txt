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

option(BUILD_RTAPI_APP_EXECUTABLE "Built the RTAPI app executable." TRUE)

if(BUILD_RTAPI_APP_EXECUTABLE)
  add_executable(rtapi_app)

  set(SOURCE_FILES src/rtapi_app.cc)
  set(PRIVATE_HEADER_FILES src/rtapi_app_module.hh)

  target_sources(
    rtapi_app
    PRIVATE ${SOURCE_FILES} ${PRIVATE_HEADER_FILES}
            $<TARGET_PROPERTY:rtapi_support,INTERFACE_SOURCES>
            $<TARGET_PROPERTY:rtapi_compat,INTERFACE_SOURCES>)

  target_include_directories(
    rtapi_app
    PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            $<TARGET_PROPERTY:rtapi_compat,INTERFACE_INCLUDE_DIRECTORIES>)

  pkg_check_modules(UUID "uuid" REQUIRED IMPORTED_TARGET)

  # Linking against Runtime just because of the config.h file? Adding
  # Runtime_api only because originally makefile was not linking in the whole
  # 'libhalulapi.so'
  target_link_libraries(
    rtapi_app
    PRIVATE mkini
            runtime_memory_api
            machinetalk
            runtime_api
            hal_api
            syslog_async
            PkgConfig::UUID
            ${CMAKE_DL_LIBS})

  target_compile_definitions(rtapi_app PRIVATE "ULAPI")

  # The relative difference should be the same for both for build BINARY tree
  # and for installed tree, but if user overrides the *_OUTPUT directories it is
  # not, compute both variants
  file(RELATIVE_PATH BUILD_RELATIVE_ORIGIN_MANAGED_MODULES
       ${MACHINEKIT_HAL_INTERNAL_EXECUTABLE_OUTPUT_DIRECTORY} 
       ${MACHINEKIT_HAL_MANAGED_MODULE_OUTPUT_DIRECTORY})
  file(RELATIVE_PATH BUILD_RELATIVE_ORIGIN_LIBRARIES
       ${MACHINEKIT_HAL_INTERNAL_EXECUTABLE_OUTPUT_DIRECTORY} 
       ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
  file(RELATIVE_PATH BUILD_RELATIVE_ORIGIN_MANAGED_MODULES
       ${MACHINEKIT_HAL_INTERNAL_EXECUTABLE_FULL_INSTALL_DIRECTORY} 
       ${MACHINEKIT_HAL_MANAGED_FULL_INSTALL_MODULE_DIRECTORY})
  file(RELATIVE_PATH BUILD_RELATIVE_ORIGIN_LIBRARIES
       ${MACHINEKIT_HAL_INTERNAL_EXECUTABLE_FULL_INSTALL_DIRECTORY} 
       ${MACHINEKIT_HAL_LIBRARY_FULL_INSTALL_DIRECTORY})

  list(APPEND RTAPI_APP_BUILD_RPATH "$ORIGIN"
       "$ORIGIN/${BUILD_RELATIVE_ORIGIN_MANAGED_MODULES}"
       "$ORIGIN/${BUILD_RELATIVE_ORIGIN_LIBRARIES}")
  list(APPEND RTAPI_APP_INSTALL_RPATH "$ORIGIN"
       "$ORIGIN/${INSTALL_RELATIVE_ORIGIN_MANAGED_MODULES}"
       "$ORIGIN/${INSTALL_RELATIVE_ORIGIN_LIBRARIES}")

  set_target_properties(
    rtapi_app
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY
               "${MACHINEKIT_HAL_INTERNAL_EXECUTABLE_OUTPUT_DIRECTORY}"
               BUILD_RPATH "${RTAPI_APP_BUILD_RPATH}"
               INSTALL_RPATH "${RTAPI_APP_INSTALL_RPATH}")
endif()
