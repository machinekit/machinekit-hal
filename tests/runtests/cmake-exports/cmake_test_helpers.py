#####################################################################
# Description:  cmake_test_helpers.py
#
#               This file, 'cmake_test_helpers.py', implements functions used
#               for testing CMake export system for Machinekit-HAL for both
#               in-source and out-of-source building of extension modules,
#               libraries and executables.
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

import pathlib
import copy
import tempfile
import subprocess
import re

from typing import List

basic_testing_cmake = """
cmake_minimum_required(VERSION 3.22)

project(Machinekit-HALRuntestCMake{test_name} LANGUAGES C)

find_package(
  Machinekit-HAL
  COMPONENTS {required_components}
  REQUIRED)

foreach(
  _target {tested_targets})
  if(TARGET ${{_target}})
    message("TARGET ${{_target}} found.")
  else()
    message("TARGET ${{_target}} NOT found!")
  endif()
endforeach()

foreach(
  _command {tested_commands})
  if(COMMAND ${{_command}})
    message("COMMAND ${{_command}} found.")
  else()
    message("COMMAND ${{_command}} NOT found!")
  endif()
endforeach()
"""


def verify_cmake_targets(
        components: List[str],
        defined_targets: List[str] = [],
        undefined_targets: List[str] = [],
        defined_commands: List[str] = [],
        undefined_commands: List[str] = [],
        directory_path: pathlib.Path = None,
        test_name: str = 'componentTest'
):
    if defined_targets is None:
        defined_targets = []
    if undefined_targets is None:
        undefined_targets = []
    if defined_commands is None:
        defined_commands = []
    if undefined_commands is None:
        undefined_commands = []

    required_components = ' '.join(components)
    tested_targets = ';'.join(defined_targets+undefined_targets)
    tested_commands = ';'.join(defined_commands+undefined_commands)
    cmake_directory = pathlib.Path(tempfile.mkdtemp(
        suffix=f'cmake_{test_name}', prefix='mkh_runtest', dir=directory_path))

    cmakefile = basic_testing_cmake.format(
        test_name=test_name,
        required_components=required_components,
        tested_targets=tested_targets,
        tested_commands=tested_commands)

    with open(cmake_directory / 'CMakeLists.txt', 'w')as f:
        f.write(cmakefile)

    print(
        f'Generated CMakeLists.txt file:\n=============\n{cmakefile}\n=============')

    try:
        cmake_output = (subprocess.check_output(
            ['cmake',
             '-S', str(cmake_directory),
             '-B', str(cmake_directory), ],
            stderr=subprocess.STDOUT)).decode('utf-8')
    except subprocess.CalledProcessError as e:
        print(f'Subprocess call with parameters: {e.cmd}\n'
              f'returned output:\n{e.output.decode("utf-8") if isinstance(e.output, bytes) else e.output}\n'
              f'and error output:\n{e.stderr.decode("utf-8") if isinstance(e.stderr, bytes) else e.stderr}\n!!!!!!!!')
        raise RuntimeError(
            f'Call to external CMake process failed with return code {e.returncode}!')

    print(
        f'CMake run output:\n**************\n{cmake_output}\n**************')

    def check_output_for_objects(
        searched_objects: List[str],
        pattern_string: str
    ):
        compiled_regex = re.compile(pattern_string)

        _available_matches = compiled_regex.findall(cmake_output)

        print(
            f'For regex pattern "{compiled_regex.pattern}" found'
            f' in the CMake output: {_available_matches}')

        if not _available_matches:
            raise RuntimeError(
                f'No matches for regex "{compiled_regex.pattern}" found!')

        leftover_defined_objects = copy.copy(searched_objects)
        for _match in _available_matches:
            if _match in leftover_defined_objects:
                leftover_defined_objects.remove(_match)

        if leftover_defined_objects:
            raise RuntimeError(
                f'Defined objects not found: {leftover_defined_objects}')

    if defined_targets:
        check_output_for_objects(
            defined_targets, r'TARGET (?P<target>[\w:]+) found.')

    if undefined_targets:
        check_output_for_objects(
            undefined_targets, r'TARGET (?P<target>[\w:]+) NOT found!')

    if defined_commands:
        check_output_for_objects(
            defined_commands, r'COMMAND (?P<target>[\w:]+) found.')

    if undefined_commands:
        check_output_for_objects(
            undefined_commands, r'COMMAND (?P<target>[\w:]+) NOT found!')
