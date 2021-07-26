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
  check_include_files("sys/io.h" SYS_IO_AVAILABLE)
endif()

if(SYS_IO_AVAILABLE)
  option(BUILD_PCI_WRITE_EXECUTABLE "Built the PCI write executable." TRUE)

  if(BUILD_PCI_WRITE_EXECUTABLE)
    find_program(CHOWN "chown" REQUIRED)
    find_program(CHMOD "chmod" REQUIRED)
    add_executable(pci_write)

    set(SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/pci_write.c)

    target_sources(pci_write PRIVATE ${SOURCE_FILES})

    target_link_libraries(pci_write PRIVATE upci)

    set_target_properties(
      pci_write
      PROPERTIES RUNTIME_OUTPUT_DIRECTORY
                 "${MACHINEKIT_HAL_INTERNAL_EXECUTABLE_OUTPUT_DIRECTORY}")

    # Special SETUID target used only for running from BUILD tree
    add_custom_target(
      pci_write_setuid
      COMMAND "${CHOWN}" "0:0" "$<TARGET_FILE:pci_write>"
      COMMAND "${CHMOD}" "4755" "$<TARGET_FILE:pci_write>"
      COMMENT "Setting the SETUID rules on target 'pci_write'.")
    add_dependencies(pci_write_setuid pci_write)
    if(TARGET setuid)
      add_dependencies(setuid pci_write_setuid)
    endif()

    install(
      TARGETS pci_write
      PERMISSIONS
        OWNER_READ
        OWNER_EXECUTE
        GROUP_READ
        GROUP_EXECUTE
        WORLD_READ
        WORLD_EXECUTE
        SETUID
        SETGID
      RUNTIME DESTINATION "${MACHINEKIT_HAL_INTERNAL_EXECUTABLE_DIRECTORY}"
              COMPONENT MachinekitHAL_Executable_PCI_Write_Binaries)

    cpack_add_component(
      MachinekitHAL_Executable_PCI_Write_Binaries
      GROUP MachinekitHAL_Executable_PCI_Write
      DEPENDS MachinekitHAL_Library_UPCI_Libraries)

    # Specification of artifacts placement in package tree
    cpack_add_component_group(
      MachinekitHAL_Executable_PCI_Write
      PARENT_GROUP MachinekitHAL_Package_Base_Executables)
  endif()
endif()
