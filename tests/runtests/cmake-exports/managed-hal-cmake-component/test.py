#####################################################################
# Description:  test.py
#
#               This file, 'test.py', implements a single CMake based
#               test of extended Machinekit-HAL buildsystem. (Primarily
#               importing previously exported targets.)
#
# Copyright (C) 2022    Jakub Fišer  <jakub DOT fiser AT eryaf DOT com>
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

import importlib.util
import pathlib
import sys

current_directory = pathlib.Path(__file__).resolve().parent

spec = importlib.util.spec_from_file_location(
    "cmake_test_helpers",
    current_directory.parent / 'cmake_test_helpers.py')
helpers = importlib.util.module_from_spec(spec)
spec.loader.exec_module(helpers)


def main() -> int:
    all_components = ['Managed-HAL']
    expected_targets = [
        'Machinekit::HAL::managed_hal',
    ]
    unexpected_targets = [
        'Machinekit::HAL::unmanaged_hal',
    ]
    expected_commands = [
        'export_rtapi_symbols',
    ]
    unexpected_commands = [
        'unknown_function',
    ]

    try:
        helpers.verify_cmake_targets(
            all_components, expected_targets, unexpected_targets, None, None, current_directory)

    except Exception as e:
        print(e)
        return -1

    return 0


if __name__ == '__main__':
    sys.exit(main())
