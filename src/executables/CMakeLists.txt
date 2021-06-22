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

add_subdirectory(comp)
add_subdirectory(decode_protobuf)
add_subdirectory(encdec)
add_subdirectory(fix_uuid)
add_subdirectory(halcmd)
add_subdirectory(halrun)
add_subdirectory(haltalk)
add_subdirectory(inivar)
add_subdirectory(instcomp)
add_subdirectory(linmove)
add_subdirectory(messagebus)
add_subdirectory(mksocmemio)
add_subdirectory(mklauncher)
add_subdirectory(mkwrapper)
add_subdirectory(msgd)
add_subdirectory(mutexwatch)
add_subdirectory(npbdecode)
add_subdirectory(pci_read)
add_subdirectory(pci_write)
add_subdirectory(position)
add_subdirectory(ppioctl)
add_subdirectory(pru_debug)
add_subdirectory(rawread)
add_subdirectory(realtime)
add_subdirectory(rtapi_app)
add_subdirectory(rtprintf)
add_subdirectory(sizes)
add_subdirectory(test_rtapi_vsnprinf)
add_subdirectory(unionread)
add_subdirectory(varint)
add_subdirectory(webtalk)
