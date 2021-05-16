#####################################################################
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
######################################################################

option(BUILD_PCI_WRITE_EXECUTABLE "Built the PCI write executable." TRUE)

if(BUILD_PCI_WRITE_EXECUTABLE)
# if(PCI)
add_executable(pci_write)

set(SOURCE_FILES src/pci_write.c)

target_sources(pci_write PRIVATE ${SOURCE_FILES})

target_link_libraries(pci_write PRIVATE upci)

set_target_properties(pci_write PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${MACHINEKIT_HAL_INTERNAL_EXECUTABLE_OUTPUT_DIRECTORY}")
#endif()
endif()
