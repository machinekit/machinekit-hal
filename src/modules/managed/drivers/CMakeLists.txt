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

add_subdirectory(chip)
add_subdirectory(hal_arm335xQEP)
add_subdirectory(hal_ax5214h)
add_subdirectory(hal_bb_gpio)
add_subdirectory(hal_evoreg)
add_subdirectory(hal_gm)
add_subdirectory(hal_h3_gpio)
add_subdirectory(hal_motenc)
add_subdirectory(hal_p260c)
add_subdirectory(hal_parport)
add_subdirectory(hal_ppmc)
add_subdirectory(hal_pru_generic)
add_subdirectory(hal_pru_generic_2)
add_subdirectory(hal_raspi_gpio)
add_subdirectory(hal_skeleton)
add_subdirectory(hal_speaker)
add_subdirectory(hal_tiro)
add_subdirectory(hal_vti)
add_subdirectory(hal_zed_can)
add_subdirectory(hal_zed_gpio)
add_subdirectory(hostmot2)
add_subdirectory(mesa_7i65)
add_subdirectory(mesa_uart)
add_subdirectory(opto_ac5)
add_subdirectory(pci_8255)
add_subdirectory(plc720)
add_subdirectory(serport)
add_subdirectory(servo_to_go)
add_subdirectory(uparport)
