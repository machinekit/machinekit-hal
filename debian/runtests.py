#!/usr/bin/env python3
# -*- coding: utf-8 -*-

#####################################################################
# Description:  runtests.py
#
#               This file, 'runtests.py', runs Machinekit-HAL's
#               runtests script
#
# Copyright (C) 2020 -   Jakub Fišer  <jakub DOT fiser AT eryaf DOT com>
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
import stat
import shutil
import pathlib
import tempfile
from typing import Union, List

import importlib.util
spec = importlib.util.spec_from_file_location(
    "machinekit_hal_script_helpers",
    "{0}/support/python/machinekit_hal_script_helpers.py".format(
        os.path.realpath(
            "{0}/..".format(os.path.dirname(os.path.abspath(__file__)))
        )))
helpers = importlib.util.module_from_spec(spec)
spec.loader.exec_module(helpers)

# TODO: Clean this up!


class Build():
    Generators = {
        "Unix Makefiles": ["make", "M", "unix"],
        "Ninja Multi-Config": ["Ninja", "N", "Ninja Multi-Config"],
    }
    Configs = [
        "Debug",
        "Release",
        "RelWithDebInfo",
    ]

    def __init__(self,
                 source_directory: Union[str, pathlib.Path],
                 build_directory: Union[str, pathlib.Path],
                 generator: str,
                 configs: Union[List[str], str]):
        if not isinstance(source_directory, pathlib.Path):
            source_directory = pathlib.Path(source_directory)
        if not isinstance(build_directory, pathlib.Path):
            build_directory = pathlib.Path(build_directory)

        if generator not in Build.Generators.keys():
            raise ValueError(
                f"Generator has to be one of {Build.Generators}!")
        self.generator = generator

        if isinstance(configs, str):
            configs = list(configs)
        if not set(configs) <= set(Build.Configs):
            raise ValueError(
                f"Configs must be in {Build.Configs}!")
        self.configs = configs

        # Possible to be run from anywhere in Machinekit-HAL's repository tree
        self.source_directory = helpers.NormalizeMachinekitHALPath(
            source_directory)()

        self.build_directory = build_directory

        if not self.build_directory.is_absolute():
            self.build_directory = self.build_directory.resolve()
        if not self.build_directory.is_dir():
            self.build_directory.mkdir()  # Throws in case Path is another object

    def disable_zeroconf(self) -> None:
        ini_files = []

        ini_postfix = "etc/machinekit/hal/machinekit.ini"
        if self.generator == "Ninja Multi-Config":
            for prefix in self.configs:
                ini_files.append(self.build_directory / prefix / ini_postfix)
        elif self.generator == "Unix Makefiles":
            ini_files.append(self.build_directory / ini_postfix)

        new_ini_items = "ANNOUNCE_IPV4=0\nANNOUNCE_IPV6=0\n"

        for ini_file in ini_files:
            ini_file.chmod(stat.S_IWUSR | stat.S_IRUSR)
            with open(ini_file, "a") as writer:
                writer.write(new_ini_items)

    def _envrc_path(self, config: str) -> pathlib.Path:
        if not config or config not in self.configs:
            raise ValueError("Config for which .envrc is required must be of "
                             f"previously configured: {self.configs}")
        if self.generator == "Ninja Multi-Config":
            return self.build_directory / config / ".envrc"
        else:
            return self.build_directory / ".envrc"

    def _prepare_run_runtests_executable(self, new_path: pathlib.Path) -> pathlib.Path:
        run_runtests = self.source_directory / "tests" / "run_runtests"
        new_run_runtests = new_path / "run_runtests"
        shutil.copyfile(run_runtests, new_run_runtests)
        new_run_runtests.chmod(stat.S_IWUSR | stat.S_IRUSR | stat.S_IXUSR)
        return new_run_runtests

    def run_runtests(self, test_path: pathlib.Path = None) -> None:
        if test_path is None:
            test_path = self.source_directory / "tests" / "runtests"
        else:
            test_path = test_path if isinstance(
                test_path, pathlib.Path) else pathlib.Path(test_path)
        for config in self.configs:
            test_directory = pathlib.Path(tempfile.mkdtemp(suffix=config))
            destination = test_directory / "runtests"
            print(
                f"--->Tests for config {config} are run in directory {destination}.<---"
                "\n====================================")
            shutil.copytree(test_path, destination, ignore=shutil.ignore_patterns(
                'pipe-*', 'result', 'strerr'))
            run_runtests = self._prepare_run_runtests_executable(
                test_directory)
            bash_command_string = f". {self._envrc_path(config)}; {run_runtests} {destination}"
            sh.bash("-c", bash_command_string,
                    _out=sys.stdout.buffer, _err=sys.stderr.buffer)

    def run_ctests(self) -> None:
        for config in self.configs:
            sh.ctest("-C", config, _cwd=self.build_directory,
                     _out=sys.stdout.buffer, _err=sys.stderr.buffer)

    # def run_python_tests(self: object) -> None:
    #    bash_command_string = ". {0}; ./nosetests/runpytest.sh".format(
    #        self.rip_environment_path)
    #    sh.bash("-c", bash_command_string,
    #            _cwd=self.normalized_path,
    #            _out=sys.stdout.buffer,
    #            _err=sys.stderr.buffer)

    def configure(self,
                  cache: dict = dict()) -> None:
        toolchain_file = self.source_directory / \
            "debian" / "debianMultiarchToolchain.cmake"
        if "CMAKE_TOOLCHAIN_FILE" not in cache:
            cache.update(dict(CMAKE_TOOLCHAIN_FILE=toolchain_file))
        cache_list = [f"-D{key}={value}" for key,
                      value in cache.items()]

        sh.cmake("-S", self.source_directory,
                 "-B", self.build_directory,
                 "-G", self.generator,
                 *cache_list,
                 _out=sys.stdout.buffer)

    def build(self,
              target: str = None) -> None:
        number_of_cores_string = sh.nproc(_tty_out=False).strip()

        build_additional = list()
        if target is not None:
            build_additional.append(["--target", target])

        for config in self.configs:
            sh.cmake("--build", self.build_directory,
                     "-j", number_of_cores_string,
                     "--verbose",
                     "--config", config,
                     *build_additional,
                     _out=sys.stdout.buffer)

        # sh.Command(autogen_path)(_cwd=src_directory, _out=sys.stdout.buffer)
        # configure_path = "{0}/configure".format(src_directory)
        # sh.Command(configure_path)(_cwd=src_directory, _out=sys.stdout.buffer)
        # sh.make("-j", number_of_cores_string,
        #        _cwd=src_directory, _out=sys.stdout.buffer)
        # sh.sudo("-S", "make", "setuid", _in=self.sudo_password,
        #        _cwd=src_directory, _out=sys.stdout.buffer)

    def test_for_sudo(self: object) -> bool:
        try:
            sh.dpkg_query("-W", "sudo", _tty_out=False)
            return True
        except Exception:
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
        except Exception:
            return False


def main(args):
    """ Main entry point of the app """
    exception = False
    try:
        build_script = Build(
            args.source_path,
            args.build_path if args.build_path else f"{args.source_path}/build",
            args.generator,
            args.configs)
        if build_script.am_i_root():
            raise ValueError(
                "This script cannot be run under the 'root' user.")
        if not build_script.test_for_sudo():
            raise ValueError(
                "The 'sudo' executable has to be installed.")
        if not build_script.test_sudo_rights(args.sudo_password):
            raise ValueError(
                "The user has to be able to use 'sudo'.")

        if not args.no_configure:
            build_script.configure()
        if not args.no_build:
            build_script.build()
        # or args.no_python_tests):
        if not (args.no_runtests or args.no_ctests):
            build_script.disable_zeroconf()
        if not args.no_runtests:
            try:
                build_script.run_runtests(args.test_path)
            except Exception as e:
                exception = True
                print(e)
                print("Run of Machinekit-HAL's Runtests FAILED!")
        if not args.no_ctests:
            try:
                build_script.run_ctests()
            except Exception as e:
                exception = True
                print(e)
                print("Run of Machinekit-HAL's CTests FAILED!")
        # if not args.no_python_tests:
        #    build_script.run_python_tests()
    except ValueError as e:
        print(e)
        exception = True

    if exception:
        print("Machinekit-HAL test suite FAILED!")
        sys.exit(1)
    
    print("Machinekit-HAL test suite ran successfully!")


class SelectGeneratorAction(argparse.Action):
    def normalize_generator(self, value: str) -> str:
        for key, values in Build.Generators.items():
            for alias in values:
                if alias == value:
                    return key
        raise argparse.ArgumentError(
            self, f"Generator {value} is not valid value!")

    def __call__(self, parser, namespace, values, option_string=None):
        if type(values) == list:
            raise argparse.ArgumentError(
                self, "Only one generator can be specified!")

        generator = self.normalize_generator(values)

        setattr(namespace, self.dest, generator)


if __name__ == "__main__":
    """ This is executed when run from the command line """
    parser = argparse.ArgumentParser(
        description="Build Machinekit-HAL and run Runtests on it")

    # Optional argument for path to Machinekit-HAL repository
    parser.add_argument("-p",
                        "--source-path",
                        action=helpers.PathExistsAction,
                        dest="source_path",
                        default=os.getcwd(),
                        metavar="PATH",
                        help="Path to root of Machinekit-HAL repository")

    # Optional argument for Debian host architecture
    parser.add_argument("-b",
                        "--build-path",
                        action="store",
                        dest="build_path",
                        default=None,
                        metavar="PATH",
                        help="Path to the build tree of Machinekit-HAL")

    parser.add_argument("-g",
                        "--generator",
                        action=SelectGeneratorAction,
                        dest="generator",
                        choices=[alias for aliases in Build.Generators.values()
                                 for alias in aliases],
                        default=list(Build.Generators.keys())[0],
                        metavar="GENERATOR",
                        help="Generator for which create the build files")

    parser.add_argument("-c",
                        "--config",
                        action="store",
                        dest="configs",
                        nargs="+",
                        choices=Build.Configs,
                        default=[Build.Configs[0]],
                        metavar="CONFIG",
                        help="Generator for which create the build files")

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

    parser.add_argument("--no-configure",
                        action="store_true",
                        help="Do not configure and generate the CMake buildsystem")

    parser.add_argument("--no-build",
                        action="store_true",
                        help="Do not build")

    parser.add_argument("--no-runtests",
                        action="store_true",
                        help="Do not run 'runtests'")

    parser.add_argument("--no-ctests",
                        action="store_true",
                        help="Do not run CMake CTest tests")

    # parser.add_argument("--no-python-tests",
    #                    action="store_true",
    #                    help="Do not run python tests")

    args = parser.parse_args()

    main(args)
