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

option(BUILD_MUX2V2_MANAGED_MODULE "Built the Mux2 V2 HAL managed module." TRUE)

if(BUILD_MUX2V2_MANAGED_MODULE)
  include(MachinekitHALInstcompFunction)

  add_instantiatable_module(
    mux2v2
    SOURCE
    src/mux2v2.icomp
    OUTPUT_DIRECTORY
    "${MACHINEKIT_HAL_MANAGED_MODULE_OUTPUT_DIRECTORY}"
    INSTALL_DIRECTORY
    "${MACHINEKIT_HAL_MANAGED_MODULE_DIRECTORY}"
    CPACK_OUTPUT_GROUP
    MachinekitHAL_Package_Base_Managed_Modules_Components)

  add_dependencies(mux2v2 instcomp_executable)
endif()
