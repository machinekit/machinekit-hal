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

add_subdirectory(adxl345)
add_subdirectory(gpio_mcp23017)
add_subdirectory(gs2_vfd)
add_subdirectory(gy68)
add_subdirectory(huanyang_vfd)
add_subdirectory(mb2hal)
add_subdirectory(pwm_pca9685)
add_subdirectory(shuttleexpress)
add_subdirectory(temp_ads7828)
add_subdirectory(temp_atlas)
add_subdirectory(temp_bbb)
add_subdirectory(vfdb_vfd)
add_subdirectory(xhc_hb04)
add_subdirectory(xhc_whb04b_6)
