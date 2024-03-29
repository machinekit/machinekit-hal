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

option(BUILD_MSGD_EXECUTABLE "Built the MSGd executable." TRUE)

if(BUILD_MSGD_EXECUTABLE)
  add_executable(msgd)

  set(SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/rtapi_msgd.cc)

  target_sources(msgd PRIVATE ${SOURCE_FILES})

  pkg_check_modules(UUID "uuid" REQUIRED IMPORTED_TARGET)
  pkg_check_modules(CZMQ "libczmq" REQUIRED IMPORTED_TARGET)
  pkg_check_modules(ZMQ "libzmq" REQUIRED IMPORTED_TARGET)

  find_library(ATOMIC NAMES "atomic" "libatomic" "libatomic.so"
                            "libatomic.so.1" REQUIRED)
  find_library(RT NAMES "rt" "librt" "librt.so" REQUIRED)

  target_link_libraries(
    msgd
    PRIVATE unmanaged_runtime
            unmanaged_hal
            runtime_memory_api
            mkini
            machinetalk
            syslog_async
            PkgConfig::UUID
            PkgConfig::CZMQ
            PkgConfig::ZMQ
            ${CMAKE_DL_LIBS}
            ${ATOMIC}
            ${RT})

  set_target_properties(
    msgd
    PROPERTIES OUTPUT_NAME "rtapi_msgd"
               PREFIX ""
               RUNTIME_OUTPUT_DIRECTORY
               "${MACHINEKIT_HAL_INTERNAL_EXECUTABLE_OUTPUT_DIRECTORY}")

  install(TARGETS msgd
          RUNTIME DESTINATION "${MACHINEKIT_HAL_INTERNAL_EXECUTABLE_DIRECTORY}"
                  COMPONENT MachinekitHAL_Executable_Msgd_Binaries)

  cpack_add_component(
    MachinekitHAL_Executable_Msgd_Binaries
    GROUP MachinekitHAL_Executable_Msgd
    DEPENDS MachinekitHAL_Library_Runtime_Libraries
            MachinekitHAL_Library_HAL_Libraries
            MachinekitHAL_Library_Runtime_Memory_API_Libraries
            MachinekitHAL_Library_Machinetalk_Libraries
            MachinekitHAL_Library_MKini_Libraries
            MachinekitHAL_Library_SysLog_Async_Libraries)

  # Specification of artifacts placement in package tree
  cpack_add_component_group(MachinekitHAL_Executable_Msgd
                            PARENT_GROUP MachinekitHAL_Package_Base_Executables)
endif()
