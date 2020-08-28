#!/usr/bin/env python3
# -*- coding: utf-8 -*-

#####################################################################
# Description:  ripruntests.py
#
#               This file, 'ripruntests.py', runs Machinekit-HAL's
#               runtests script
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
Script for running Machinekit-HAL runtests
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
    "{0}/scripts/machinekit_hal_script_helpers.py".format(
        os.path.realpath(
            "{0}/..".format(os.path.dirname(os.path.abspath(__file__)))
        )))
helpers = importlib.util.module_from_spec(spec)
spec.loader.exec_module(helpers)


class Ripruntests_script():
    def __init__(self: object, path):
        self.normalized_path = helpers.NormalizeMachinekitHALPath(path)()

    def run_runtests(self: object, test_path) -> None:
        rip_ini_file = "{0}/etc/machinekit/machinekit.ini".format(
            self.normalized_path)
        new_ini_items = "ANNOUNCE_IPV4=0\nANNOUNCE_IPV6=0\n"
        with open(rip_ini_file, "a") as writer:
            writer.write(new_ini_items)
        if test_path is None:
            test_path = "{0}/tests".format(self.normalized_path)
        rip_environment_path = "{0}/scripts/rip-environment".format(
            self.normalized_path)
        bash_command_string = ". {0}; runtests {1}".format(
            rip_environment_path, test_path)
        sh.bash("-c", bash_command_string,
                _cwd=self.normalized_path, _out=sys.stdout.buffer, _err=sys.stderr.buffer)

    def build_rip(self: object) -> None:
        src_directory = "{0}/src".format(self.normalized_path)
        number_of_cores_string = sh.nproc(_tty_out=False).strip()
        autogen_path = "{0}/autogen.sh".format(src_directory)
        sh.Command(autogen_path)(_cwd=src_directory, _out=sys.stdout.buffer)
        configure_path = "{0}/configure".format(src_directory)
        sh.Command(configure_path)(_cwd=src_directory, _out=sys.stdout.buffer)
        sh.make("-j", number_of_cores_string,
                _cwd=src_directory, _out=sys.stdout.buffer)
        sh.sudo("-S", "make", "setuid", _in=self.sudo_password,
                _cwd=src_directory, _out=sys.stdout.buffer)

    def test_for_sudo(self: object) -> bool:
        try:
            sh.dpkg_query("-W", "sudo", _tty_out=False)
            return True
        except:
            return False

    def am_i_root(self: object) -> bool:
        uid = os.getuid()
        if uid in [0]:
            return True
        return False

    def test_sudo_rights(self: object, sudo_password) -> bool:
        # Test for sudo rights if some password was specified
        try:
            sh.sudo("-S", "true", _in=sudo_password)
            self.sudo_password = sudo_password
            return True
        except:
            return False


def main(args):
    """ Main entry point of the app """
    try:
        ripruntests_script = Ripruntests_script(args.path)
        if ripruntests_script.am_i_root():
            raise ValueError(
                "This script cannot be run under the 'root' user.")
        if not ripruntests_script.test_for_sudo():
            raise ValueError(
                "The 'sudo' executable has to be installed.")
        if not ripruntests_script.test_sudo_rights(args.sudo_password):
            raise ValueError(
                "The user has to be able to use 'sudo'.")

        ripruntests_script.build_rip()
        ripruntests_script.run_runtests(args.test_path)
        print("Machinekit-HAL regression tests ran successfully!")
    except ValueError as e:
        print(e)
        sys.exit(1)


if __name__ == "__main__":
    """ This is executed when run from the command line """
    parser = argparse.ArgumentParser(
        description="Build Machinekit-HAL RIP version and run Runtests on it")

    # Optional argument for path to Machinekit-HAL repository
    parser.add_argument("-p",
                        "--path",
                        action=helpers.PathExistsAction,
                        dest="path",
                        default=os.getcwd(),
                        help="Path to root of Machinekit-HAL repository")
    # Optional argument for Debian host architecture
    parser.add_argument("-t",
                        "--tests-path",
                        dest="test_path",
                        action=helpers.PathExistsAction,
                        metavar="PATH",
                        help="Path with test definitions.")

    parser.add_argument("-s",
                        "--sudo-password",
                        dest="sudo_password",
                        action="store",
                        metavar="PASSWORD",
                        help="Password for usage with sudo command.")

    args = parser.parse_args()

    main(args)
