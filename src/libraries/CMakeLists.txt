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

# Special bootstrapping step
add_subdirectory(runtime_bootstrap)
add_subdirectory(symbol_visibility)

add_subdirectory(ads7828)
add_subdirectory(config_service)
add_subdirectory(cycompat)
add_subdirectory(cyhal)
add_subdirectory(cyruntime)
add_subdirectory(cyshmcommon)
add_subdirectory(export_package)
add_subdirectory(hal)
add_subdirectory(hal_command)
add_subdirectory(halfile)
add_subdirectory(hal_glib)
add_subdirectory(hal_parport)
add_subdirectory(halscope_memory)
add_subdirectory(hostmot2_bitfile)
add_subdirectory(launcher)
add_subdirectory(linux_event)
add_subdirectory(machinetalk)
add_subdirectory(mcp23017)
add_subdirectory(misc_gtk)
add_subdirectory(mkconfig)
add_subdirectory(mkini)
add_subdirectory(nanopb)
add_subdirectory(nanopb_definitions)
add_subdirectory(pb2json)
add_subdirectory(pca9685)
add_subdirectory(proto)
add_subdirectory(pru_app_loader)
add_subdirectory(pru_defines)
add_subdirectory(pyhalmodule)
add_subdirectory(raspi_cpuinfo)
add_subdirectory(rtapi_compat)
add_subdirectory(rtapi_pci)
add_subdirectory(rtapi_support)
add_subdirectory(runtime)
add_subdirectory(runtime_math)
add_subdirectory(runtime_memory)
add_subdirectory(samstr_memdefs)
add_subdirectory(service)
add_subdirectory(service_discovery)
add_subdirectory(syslog_async)
add_subdirectory(upci)
add_subdirectory(user_pci)
