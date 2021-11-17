# No shebang specification!
# -*- coding: utf-8 -*-

#####################################################################
# Description:  wheel_installer.py
#
#               This file, 'wheel_installer.py', implements the basic Python
#               wheel installer CLI for the 'installer' python package
#               as needed for the Machinekit-HAL CMake based buildsystem
#
#               IMPORTANT:
#               This (even though it has external dependency) should be run
#               as /path/to/python3/interpreter wheel_installer.py (or better
#               yet execute_process(Python::Interpreter whhel_installer.py))
#               and should NOT be installed as an packaged module!
#
# Copyright (C) 2021 -   Jakub Fišer  <jakub DOT fiser AT eryaf DOT com>
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

"""
Script for installing Machinekit-HAL specific Python3 wheels to arbitrary
directories.

Used mainly for the Machinekit-HAL CMake based buildsystem workflow.
"""

from installer.sources import WheelFile
from installer.destinations import SchemeDictionaryDestination
from installer import install
__license__ = "LGPL 2.1"
__version__ = "1.0.0"

import argparse
import pathlib
import sys
import sysconfig


def main(args):
    """Main entry point of the app"""

    destination = SchemeDictionaryDestination(
        dict(
            stdlib=str(args.stdlib_path),
            platstdlib=str(args.platstdlib_path),
            purelib=str(args.purelib_path),
            platlib=str(args.platlib_path),
            include=str(args.include_path),
            platinclude=str(args.platinclude_path),
            scripts=str(args.scripts_path),
            data=str(args.data_path),
        ),
        interpreter=str(args.interpreter),
        script_kind="posix",
    )

    for wheel in args.wheel_path:
        with WheelFile.open(wheel) as source:
            install(
                source=source,
                destination=destination,
                # Additional metadata that is generated
                # by the installation tool.
                additional_metadata={
                    "INSTALLER": 'Machinekit-HAL Python wheel'
                    f'installer {__version__}'.encode('utf-8'),
                },
            )

        print(f'Wheel {wheel} installed')

    print("All wheels were installed!")


class wheelExistsAction(argparse.Action):
    def __call__(self, parser, args, values, option_string=None):
        if isinstance(values, str):
            values = [values]

        output = []
        for value in values:
            wheel_path = pathlib.Path(value)
            if not wheel_path.exists():
                raise argparse.ArgumentError(
                    self, f"Path {wheel_path} does not exists!")
            if wheel_path.is_dir():
                for file in wheel_path.glob('*.whl'):
                    output.append(file)
                if not output:
                    raise argparse.ArgumentError(
                        self, f"Directory {wheel_path} does not contain"
                        "any wheel!")
            else:
                if not wheel_path.is_file():
                    raise argparse.ArgumentError(
                        self, f"Path {wheel_path} is not a file!")
                if wheel_path.suffix != '.whl':
                    raise argparse.ArgumentError(
                        self, f"Path {wheel_path} doesn't have"
                        "a valid suffix!")
                output.append(wheel_path)

        setattr(args, self.dest, output)


if __name__ == "__main__":
    """This is executed when run from the command line"""
    parser = argparse.ArgumentParser(
        description="Build PathPilot Robot Docker Image used for distribution"
        "to customers.",
        formatter_class=argparse.ArgumentDefaultsHelpFormatter,
    )

    parser.add_argument(
        "wheel_path",
        action=wheelExistsAction,
        metavar="WHEEL",
        nargs='*',
        help="Path to the Python wheel file or a directory containing a wheel"
        "or wheels as direct children",
    )
    parser.add_argument(
        "--interpreter",
        "-i",
        action="store",
        type=pathlib.Path,
        metavar="INTERPRETER",
        help="Path to the Python interpreter to use for script generation",
        default=sys.executable,
    )

    default_paths = sysconfig.get_paths()

    parser.add_argument(
        "--stdlib-path",
        "-sl",
        action="store",
        type=pathlib.Path,
        metavar="STLIB",
        help="Path to the Python stdlib directory",
        default=pathlib.Path(default_paths['stdlib']),
    )
    parser.add_argument(
        "--platstdlib-path",
        "-psl",
        action="store",
        type=pathlib.Path,
        metavar="PLATSTDLIB",
        help="Path to the Python platstdlib directory",
        default=pathlib.Path(default_paths['platstdlib']),
    )
    parser.add_argument(
        "--purelib-path",
        "-pul",
        action="store",
        type=pathlib.Path,
        metavar="PURELIB",
        help="Path to the Python purelib directory",
        default=pathlib.Path(default_paths['purelib']),
    )
    parser.add_argument(
        "--platlib-path",
        "-pl",
        action="store",
        type=pathlib.Path,
        metavar="PLATLIB",
        help="Path to the Python platlib directory",
        default=pathlib.Path(default_paths['platlib']),
    )
    parser.add_argument(
        "--include-path",
        "-in",
        action="store",
        type=pathlib.Path,
        metavar="INCLUDE",
        help="Path to the Python include directory",
        default=pathlib.Path(default_paths['include']),
    )
    parser.add_argument(
        "--platinclude-path",
        "-pin",
        action="store",
        type=pathlib.Path,
        metavar="PLATINCLUDE",
        help="Path to the Python platinclude directory",
        default=pathlib.Path(default_paths['platinclude']),
    )
    parser.add_argument(
        "--scripts-path",
        "-s",
        action="store",
        type=pathlib.Path,
        metavar="SCRIPTS",
        help="Path to the Python scripts directory",
        default=pathlib.Path(default_paths['scripts']),
    )
    parser.add_argument(
        "--data-path",
        "-d",
        action="store",
        type=pathlib.Path,
        metavar="DATA",
        help="Path to the Python scripts directory",
        default=pathlib.Path(default_paths['data']),
    )

    args = parser.parse_args()

    main(args)
