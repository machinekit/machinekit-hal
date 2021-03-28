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

add_subdirectory(adxl345)
add_subdirectory(constv2)
add_subdirectory(gs2_vfd)
add_subdirectory(hal-gpio_mcp23017)
add_subdirectory(hal_gy68)
add_subdirectory(hal_input)
add_subdirectory(hal_pwm_pca9685)
add_subdirectory(halscope)
add_subdirectory(hal_storage)
add_subdirectory(hal_temp)
add_subdirectory(huanyang_vfd)
add_subdirectory(mb2hal)
add_subdirectory(meter)
add_subdirectory(remote)
add_subdirectory(ringbench)
add_subdirectory(ringdemo)
add_subdirectory(scounter)
add_subdirectory(shuttleexpress)
add_subdirectory(vfdb_vfd)
add_subdirectory(videoserver)
add_subdirectory(vismach)
add_subdirectory(xhc_hb04)
add_subdirectory(xhc_whb04b_6)
