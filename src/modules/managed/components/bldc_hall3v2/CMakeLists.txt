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

option(BUILD_BLDC_HALL3V2_MANAGED_MODULE
       "Built the BLDC HALL3 HAL managed module." TRUE)

if(BUILD_BLDC_HALL3V2_MANAGED_MODULE)
  include(MachinekitHALInstcomp)

  add_instantiatable_module(
    bldc_hall3v2 SOURCE src/bldc_hall3v2.icomp OUTPUT_DIRECTORY
    "${MACHINEKIT_HAL_MANAGED_MODULE_OUTPUT_DIRECTORY}")

  add_dependencies(bldc_hall3v2 instcomp_executable)
endif()
