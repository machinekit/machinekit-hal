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

if(NOT DEFINED SYS_IO_AVAILABLE)
  if(NOT DEFINED check_include_files)
    include(CheckIncludeFiles)
  endif()

  check_include_files("sys/io.h" SYS_IO_AVAILABLE)
endif()

if(SYS_IO_AVAILABLE)
  option(BUILD_RTAPI_PCI_LIBRARY "Built the RTAPI PCI libraries." TRUE)

  if(BUILD_RTAPI_PCI_LIBRARY)
    add_library(rtapi_pci_api INTERFACE)
    add_library(${MACHINEKIT_HAL_NAMESPACE}::rtapi_pci_api ALIAS rtapi_pci_api)
    add_library(managed_rtapi_pci SHARED)
    add_library(${MACHINEKIT_HAL_NAMESPACE}::managed_rtapi_pci ALIAS
                managed_rtapi_pci)
    add_library(unmanaged_rtapi_pci SHARED)
    add_library(${MACHINEKIT_HAL_NAMESPACE}::unmanaged_rtapi_pci ALIAS
                unmanaged_rtapi_pci)

    set(SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/rtapi_pci.c)

    set(PUBLIC_INCLUDE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include")

    set(PUBLIC_HEADER_FILES_BUILD
        ${PUBLIC_INCLUDE_DIRECTORY}/runtime/rtapi_pci.h)

    set(PUBLIC_HEADER_FILES_INSTALL
        ${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/runtime/rtapi_pci.h)

    target_sources(
      rtapi_pci_api
      PUBLIC $<BUILD_INTERFACE:${PUBLIC_HEADER_FILES_BUILD}>
             $<INSTALL_INTERFACE:${PUBLIC_HEADER_FILES_INSTALL}>
      PRIVATE ${SOURCE_FILES})

    target_sources(managed_rtapi_pci PRIVATE ${SOURCE_FILES})

    target_sources(unmanaged_rtapi_pci PRIVATE ${SOURCE_FILES})

    pkg_check_modules(LIBUDEV "libudev" REQUIRED IMPORTED_TARGET)

    target_include_directories(
      rtapi_pci_api
      INTERFACE $<BUILD_INTERFACE:${PUBLIC_INCLUDE_DIRECTORY}>
                $<INSTALL_INTERFACE:${MACHINEKIT_HAL_INTERFACE_DIRECTORY}>)

    target_link_libraries(
      managed_rtapi_pci
      PUBLIC rtapi_pci_api
      PRIVATE runtime_api runtime_config PkgConfig::LIBUDEV)

    target_link_libraries(
      unmanaged_rtapi_pci
      PUBLIC rtapi_pci_api
      PRIVATE runtime_api runtime_config PkgConfig::LIBUDEV)

    set_target_properties(
      rtapi_pci_api
      PROPERTIES PUBLIC_HEADER "${PUBLIC_HEADER_FILES_BUILD}"
                 PUBLIC_INCLUDE_DIRECTORY "${PUBLIC_INCLUDE_DIRECTORY}")

    # This is a special case because rtapi_pci TARGET is a direct dependency of
    # *_runtime TARGETs and thus the "ULAPI" or "RTAPI" compile definitions
    # cannot be transitivelly defined by linking agains the *_runtime as that
    # would create a cyclic dependency and CMake error
    #
    # TODO: Maybe there is a better solution to this mess?
    target_compile_definitions(managed_rtapi_pci PUBLIC "RTAPI")
    target_compile_definitions(unmanaged_rtapi_pci PUBLIC "ULAPI")

    set_target_properties(
      managed_rtapi_pci
      PROPERTIES SOVERSION 0
                 PREFIX "libmkh"
                 VERSION ${CMAKE_PROJECT_VERSION}
                 POSITION_INDEPENDENT_CODE TRUE)

    set_target_properties(
      unmanaged_rtapi_pci
      PROPERTIES SOVERSION 0
                 PREFIX "libmkh"
                 VERSION ${CMAKE_PROJECT_VERSION}
                 POSITION_INDEPENDENT_CODE TRUE)

    configure_file(
      ${CMAKE_CURRENT_SOURCE_DIR}/cmake/MachinekitHALUnmanaged-Runtime-PCIComponent.cmake.in
      ${MACHINEKIT_HAL_CMAKE_PACKAGE_OUTPUT_DIRECTORY}/MachinekitHALUnmanaged-Runtime-PCIComponent.cmake
      NO_SOURCE_PERMISSIONS
      @ONLY)

    configure_file(
      ${CMAKE_CURRENT_SOURCE_DIR}/cmake/MachinekitHALManaged-Runtime-PCIComponent.cmake.in
      ${MACHINEKIT_HAL_CMAKE_PACKAGE_OUTPUT_DIRECTORY}/MachinekitHALManaged-Runtime-PCIComponent.cmake
      NO_SOURCE_PERMISSIONS
      @ONLY)

    install(
      TARGETS rtapi_pci_api
      EXPORT machinekit_hal_rtapi_pci_api
      PUBLIC_HEADER DESTINATION "${MACHINEKIT_HAL_INTERFACE_DIRECTORY}/runtime"
                    COMPONENT MachinekitHAL_Library_RTAPI_PCI_API_Headers)

    install(
      TARGETS managed_rtapi_pci
      EXPORT machinekit_hal_managed_rtapi_pci
      LIBRARY
        DESTINATION "${MACHINEKIT_HAL_LIBRARY_DIRECTORY}"
        COMPONENT MachinekitHAL_Library_Managed_RTAPI_PCI_Libraries
        NAMELINK_COMPONENT MachinekitHAL_Library_Managed_RTAPI_PCI_Namelinks)

    install(
      TARGETS unmanaged_rtapi_pci
      EXPORT machinekit_hal_unmanaged_rtapi_pci
      LIBRARY
        DESTINATION "${MACHINEKIT_HAL_LIBRARY_DIRECTORY}"
        COMPONENT MachinekitHAL_Library_Unmanaged_RTAPI_PCI_Libraries
        NAMELINK_COMPONENT MachinekitHAL_Library_Unmanaged_RTAPI_PCI_Namelinks)

    install(
      EXPORT machinekit_hal_rtapi_pci_api
      DESTINATION ${MACHINEKIT_HAL_CMAKE_PACKAGE_DIRECTORY}
      NAMESPACE "${MACHINEKIT_HAL_NAMESPACE}::"
      FILE "MachinekitHALRuntimePCIAPITarget.cmake"
      COMPONENT MachinekitHAL_Library_RTAPI_PCI_API_Package_Exports)

    install(
      EXPORT machinekit_hal_managed_rtapi_pci
      DESTINATION ${MACHINEKIT_HAL_CMAKE_PACKAGE_DIRECTORY}
      NAMESPACE "${MACHINEKIT_HAL_NAMESPACE}::"
      FILE "MachinekitHALManagedRuntimePCITarget.cmake"
      COMPONENT MachinekitHAL_Library_Managed_RTAPI_PCI_Package_Exports)

    install(
      EXPORT machinekit_hal_unmanaged_rtapi_pci
      DESTINATION ${MACHINEKIT_HAL_CMAKE_PACKAGE_DIRECTORY}
      NAMESPACE "${MACHINEKIT_HAL_NAMESPACE}::"
      FILE "MachinekitHALUnmanagedRuntimePCITarget.cmake"
      COMPONENT MachinekitHAL_Library_Unmanaged_RTAPI_PCI_Package_Exports)

    install(
      FILES
        "${MACHINEKIT_HAL_CMAKE_PACKAGE_OUTPUT_DIRECTORY}/MachinekitHALUnmanaged-Runtime-PCIComponent.cmake"
      DESTINATION ${MACHINEKIT_HAL_CMAKE_PACKAGE_DIRECTORY}
      COMPONENT MachinekitHAL_Library_Unmanaged_RTAPI_PCI_Package_Exports)

    install(
      FILES
        "${MACHINEKIT_HAL_CMAKE_PACKAGE_OUTPUT_DIRECTORY}/MachinekitHALManaged-Runtime-PCIComponent.cmake"
      DESTINATION ${MACHINEKIT_HAL_CMAKE_PACKAGE_DIRECTORY}
      COMPONENT MachinekitHAL_Library_Managed_RTAPI_PCI_Package_Exports)

    export(
      EXPORT machinekit_hal_rtapi_pci_api
      NAMESPACE "${MACHINEKIT_HAL_NAMESPACE}::"
      FILE "${MACHINEKIT_HAL_CMAKE_PACKAGE_OUTPUT_DIRECTORY}/MachinekitHALRuntimePCIAPITarget.cmake"
    )

    export(
      EXPORT machinekit_hal_managed_rtapi_pci
      NAMESPACE "${MACHINEKIT_HAL_NAMESPACE}::"
      FILE "${MACHINEKIT_HAL_CMAKE_PACKAGE_OUTPUT_DIRECTORY}/MachinekitHALManagedRuntimePCITarget.cmake"
    )

    export(
      EXPORT machinekit_hal_unmanaged_rtapi_pci
      NAMESPACE "${MACHINEKIT_HAL_NAMESPACE}::"
      FILE "${MACHINEKIT_HAL_CMAKE_PACKAGE_OUTPUT_DIRECTORY}/MachinekitHALUnmanagedRuntimePCITarget.cmake"
    )

    cpack_add_component(MachinekitHAL_Library_Unmanaged_RTAPI_PCI_Libraries
                        GROUP MachinekitHAL_Library_RTAPI_PCI)
    cpack_add_component(MachinekitHAL_Library_Managed_RTAPI_PCI_Libraries
                        GROUP MachinekitHAL_Library_RTAPI_PCI)
    cpack_add_component(
      MachinekitHAL_Library_Unmanaged_RTAPI_PCI_Namelinks
      GROUP MachinekitHAL_Library_RTAPI_PCI_Development
      DEPENDS MachinekitHAL_Library_Unmanaged_RTAPI_PCI_Libraries
              MachinekitHAL_Library_RTAPI_PCI_API_Headers)
    cpack_add_component(
      MachinekitHAL_Library_Managed_RTAPI_PCI_Namelinks
      GROUP MachinekitHAL_Library_RTAPI_PCI_Development
      DEPENDS MachinekitHAL_Library_Managed_RTAPI_PCI_Libraries
              MachinekitHAL_Library_RTAPI_PCI_API_Headers)
    cpack_add_component(MachinekitHAL_Library_RTAPI_PCI_API_Headers
                        GROUP MachinekitHAL_Library_RTAPI_PCI_Development)
    cpack_add_component(
      MachinekitHAL_Library_RTAPI_PCI_API_Package_Exports
      DEPENDS MachinekitHAL_Library_RTAPI_PCI_API_Headers
      GROUP MachinekitHAL_Library_RTAPI_PCI_Development)
    cpack_add_component(
      MachinekitHAL_Library_Managed_RTAPI_PCI_Package_Exports
      DEPENDS MachinekitHAL_Library_RTAPI_PCI_API_Headers
              MachinekitHAL_Library_Managed_RTAPI_PCI_Namelinks
              MachinekitHAL_Library_Managed_RTAPI_PCI_Libraries
      GROUP MachinekitHAL_Library_RTAPI_PCI_Development)
    cpack_add_component(
      MachinekitHAL_Library_Unmanaged_RTAPI_PCI_Package_Exports
      DEPENDS MachinekitHAL_Library_RTAPI_PCI_API_Headers
              MachinekitHAL_Library_Unmanaged_RTAPI_PCI_Namelinks
              MachinekitHAL_Library_Unmanaged_RTAPI_PCI_Libraries
      GROUP MachinekitHAL_Library_RTAPI_PCI_Development)

    # Specification of artifacts placement in package tree
    cpack_add_component_group(MachinekitHAL_Library_RTAPI_PCI
                              PARENT_GROUP MachinekitHAL_Package_Base_Libraries)
    cpack_add_component_group(
      MachinekitHAL_Library_RTAPI_PCI_Development
      PARENT_GROUP MachinekitHAL_Package_Base_Libraries_Development)
  endif()
endif()
