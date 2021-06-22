#!/usr/bin/env python3
# -*- coding: utf-8 -*-

#####################################################################
# Description:  buildpackages.py
#
#               This file, 'buildpackages.py', implements scripted workflow for
#               building .deb and .ddeb native packages for Debian flavoured
#               distributions.
#
# Copyright (C) 2020    Jakub Fišer  <jakub DOT fiser AT eryaf DOT com>
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
Script for building packages for Debian styled distributions using
'dpkg' family of tools
"""

# Debian 9 Stretch, Ubuntu 18.04 Bionic and (probably) other older distributions
# need in environment LANG=C.UTF-8 (or other similar specification of encoding)
# to properly function

__license__ = "LGPL 2.1"

import argparse
import sh
import os
import sys

import importlib.util
spec = importlib.util.spec_from_file_location(
    "machinekit_hal_script_helpers",
    "{0}/support/python/machinekit_hal_script_helpers.py".format(
        os.path.realpath(
            "{0}/..".format(os.path.dirname(os.path.abspath(__file__)))
        )))
helpers = importlib.util.module_from_spec(spec)
spec.loader.exec_module(helpers)


class Buildpackages_script():
    def __init__(self: object, path, host_architecture):
        self.normalized_path = helpers.NormalizeMachinekitHALPath(path)()
        self.host_architecture = host_architecture
        self.architecture_can_be_build()

    def architecture_can_be_build(self: object) -> None:
        build_architectures = sh.dpkg(
            "--print-foreign-architectures", _tty_out=False).strip().split()
        build_architectures.append(sh.dpkg("--print-architecture",
                                           _tty_out=False).strip())
        if self.host_architecture not in build_architectures:
            raise ValueError(
                "Host architecture {} cannot be build.".format(
                    self.host_architecture))

    def build_packages(self: object):
        parent_directory = os.path.realpath(
            "{0}/..".format(self.normalized_path))
        if not os.access(parent_directory, os.W_OK):
            raise ValueError(
                "Directory {0} is not writable.".format(parent_directory))
        try:
            dpkg_buildpackage_string_arguments = ["-uc",
                                                  "-us",
                                                  "-a",
                                                  self.host_architecture,
                                                  "-B"]
            if sh.lsb_release("-cs", _tty_out=False).strip().lower() in ["stretch", "bionic"]:
                dpkg_buildpackage_string_arguments.append("-d")
            sh.dpkg_buildpackage(*dpkg_buildpackage_string_arguments,
                                 _out=sys.stdout.buffer,
                                 _err=sys.stderr.buffer,
                                 _cwd=self.normalized_path)
        except sh.ErrorReturnCode as e:
            message = "Packages cannot be build because of an error\n{0}".format(
                e)
            raise ValueError(message)


def main(args):
    """ Main entry point of the app """
    try:
        buildpackages_script = Buildpackages_script(
            args.path, args.host_architecture)
        buildpackages_script.build_packages()
        print("Packages built successfully!")
    except ValueError as e:
        print(e)
        sys.exit(1)


class HostArchitectureValidAction(argparse.Action):
    def __call__(self, parser, namespace, values, option_string=None):
        try:
            sh.dpkg_architecture("-a", values, _tty_out=False)
        except sh.ErrorReturnCode:
            raise argparse.ArgumentError(self,
                                         "Architecture {} is a not valid DPKG one.".format(values))
        setattr(namespace, self.dest, values)


if __name__ == "__main__":
    """ This is executed when run from the command line """
    parser = argparse.ArgumentParser(
        description="Build packages for Debian like distributions")

    # Optional argument for path to Machinekit-HAL repository
    parser.add_argument("-p",
                        "--path",
                        action=helpers.PathExistsAction,
                        dest="path",
                        default=os.getcwd(),
                        help="Path to root of Machinekit-HAL repository")
    # Optional argument for Debian host architecture
    parser.add_argument("-a",
                        "--host-architecture",
                        dest="host_architecture",
                        action=HostArchitectureValidAction,
                        default=sh.dpkg_architecture(
                            "-qDEB_HOST_ARCH",
                            _tty_out=False).strip(),
                        metavar="ARCHITECTURE",
                        help="Build packages for specific architecture")

    args = parser.parse_args()

    main(args)
