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
  find_program(CHOWN "chown" REQUIRED)
  find_program(CHMOD "chmod" REQUIRED)
  add_executable(rtapi_app)

  set(SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/rtapi_app.cc)
  set(PRIVATE_HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/rtapi_app_module.hh)

  target_sources(
    rtapi_app PRIVATE ${SOURCE_FILES} ${PRIVATE_HEADER_FILES}
                      $<TARGET_PROPERTY:rtapi_support,INTERFACE_SOURCES>)

  target_include_directories(
    rtapi_app PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

  pkg_check_modules(CZMQ "libczmq" REQUIRED IMPORTED_TARGET)
  pkg_check_modules(UUID "uuid" REQUIRED IMPORTED_TARGET)

  # Linking against Runtime just because of the config.h file? Adding
  # Runtime_api only because originally makefile was not linking in the whole
  # 'libhalulapi.so'
  target_link_libraries(
    rtapi_app
    PRIVATE mkini
            runtime_memory_api
            rtapi_compat
            machinetalk
            unmanaged_runtime # TODO: Check if symbols pollution from
                               # unmanaged_runtime
            # is not problematic; was runtime_api
            unmanaged_hal # TODO: Check if symbols pollution from unmanaged_hal
                          # is not problematic; was hal_api
            syslog_async
            PkgConfig::UUID
            PkgConfig::CZMQ
            ${CMAKE_DL_LIBS})

  #[[
  # The 'rtapi_app' executable will mostly run as root setuid program or
  # with CAP_SYS_NICE and  CAP_SYS_RAWIO capabilities, which means that
  # the $ORIGIN placeholder in RPATH cannot be expanded and absolute path
  # must be used, thus the program suite cannot really be relocatable
  # at this point in time
  #
  # For 'posix' flavour and no I/O, the 'rtapi_app' executable does not
  # have to run as setuid root (or with capabilities), and following
  # construct can be used:

  file(RELATIVE_PATH BUILD_RELATIVE_ORIGIN_MANAGED_MODULES
       ${MACHINEKIT_HAL_INTERNAL_EXECUTABLE_OUTPUT_DIRECTORY} 
       ${MACHINEKIT_HAL_MANAGED_MODULE_OUTPUT_DIRECTORY})
  file(RELATIVE_PATH BUILD_RELATIVE_ORIGIN_LIBRARIES
       ${MACHINEKIT_HAL_INTERNAL_EXECUTABLE_OUTPUT_DIRECTORY} 
       ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
  file(RELATIVE_PATH INSTALL_RELATIVE_ORIGIN_MANAGED_MODULES
       ${MACHINEKIT_HAL_INTERNAL_EXECUTABLE_FULL_INSTALL_DIRECTORY} 
       ${MACHINEKIT_HAL_MANAGED_FULL_INSTALL_MODULE_DIRECTORY})
  file(RELATIVE_PATH INSTALL_RELATIVE_ORIGIN_LIBRARIES
       ${MACHINEKIT_HAL_INTERNAL_EXECUTABLE_FULL_INSTALL_DIRECTORY} 
       ${MACHINEKIT_HAL_LIBRARY_FULL_INSTALL_DIRECTORY})
  ]]

  list(
    APPEND
    RTAPI_APP_BUILD_RPATH
    # "$ORIGIN/${BUILD_RELATIVE_ORIGIN_MANAGED_MODULES}"
    # "$ORIGIN/${BUILD_RELATIVE_ORIGIN_LIBRARIES}"
    "${MACHINEKIT_HAL_MANAGED_MODULE_OUTPUT_DIRECTORY}")
  list(
    APPEND
    RTAPI_APP_INSTALL_RPATH
    # "$ORIGIN/${INSTALL_RELATIVE_ORIGIN_MANAGED_MODULES}"
    # "$ORIGIN/${INSTALL_RELATIVE_ORIGIN_LIBRARIES}"
    "${MACHINEKIT_HAL_MANAGED_FULL_INSTALL_MODULE_DIRECTORY}")

  set_target_properties(
    rtapi_app
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY
               "${MACHINEKIT_HAL_INTERNAL_EXECUTABLE_OUTPUT_DIRECTORY}"
               BUILD_RPATH "${RTAPI_APP_BUILD_RPATH}"
               INSTALL_RPATH "${RTAPI_APP_INSTALL_RPATH}")

  # Special SETUID target used only for running from BUILD tree
  add_custom_target(
    rtapi_app_setuid
    COMMAND "${CHOWN}" "0:0" "$<TARGET_FILE:rtapi_app>"
    COMMAND "${CHMOD}" "4755" "$<TARGET_FILE:rtapi_app>"
    COMMENT "Setting the SETUID rules on target 'rtapi_app'.")
  add_dependencies(rtapi_app_setuid rtapi_app)
  if(TARGET setuid)
    add_dependencies(setuid rtapi_app_setuid)
  endif()

  install(
    TARGETS rtapi_app
    PERMISSIONS
      OWNER_READ
      OWNER_WRITE
      OWNER_EXECUTE
      GROUP_READ
      GROUP_EXECUTE
      WORLD_READ
      WORLD_EXECUTE
      SETUID
      SETGID
    RUNTIME DESTINATION "${MACHINEKIT_HAL_INTERNAL_EXECUTABLE_DIRECTORY}"
            COMPONENT MachinekitHAL_Executable_RTAPI_App_Binaries)

  cpack_add_component(
    MachinekitHAL_Executable_RTAPI_App_Binaries
    GROUP MachinekitHAL_Executable_RTAPI_App
    DEPENDS MachinekitHAL_Library_Runtime_Memory_API_Libraries
            MachinekitHAL_Library_MKIni_Libraries
            MachinekitHAL_Library_Machinetalk_Libraries
            MachinekitHAL_Library_Syslog_Async_Libraries
            MachinekitHAL_Managed_Module_HAL
            MachinekitHAL_Managed_Module_Runtime)

  # Specification of artifacts placement in package tree
  cpack_add_component_group(MachinekitHAL_Executable_RTAPI_App
                            PARENT_GROUP MachinekitHAL_Package_Base_Executables)
endif()
