#!/usr/bin/env python3
# # -*- coding: utf-8 -*-

#####################################################################
# Description:  machinekit_hal_script_helpers.py
#
#               This file, 'machinekit_hal_script_helpers.py', implements all
#               common functions used in Python maintenance scripts for
#               Machinekit-HAL
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

import argparse
import re
import os
import sh
import pathlib
from typing import Union

# Ubuntu 18.04 Bionic needs in environment LANG=C.UTF-8 (or other similar
# specification of encoding) to properly function


class PathExistsAction(argparse.Action):
    def test_path(self, path: str) -> str:
        if not os.path.isdir(path):
            raise argparse.ArgumentError(self,
                                         "Path {} does not exist.".format(path))
        if not os.access(path, os.W_OK):
            raise argparse.ArgumentError(self,
                                         "Path {} cannot be written to.".format(path))
        return os.path.realpath(os.path.abspath(path.rstrip(os.sep)))

    def __call__(self, parser, namespace, values, option_string=None):
        if type(values) == list:
            folders = map(self.test_path, values)
        else:
            folders = self.test_path(values)

        setattr(namespace, self.dest, folders)


class NormalizeMachinekitHALPath():
    def __init__(self, path: Union[pathlib.Path, str]):
        self.path = path if isinstance(
            path, pathlib.Path) else pathlib.Path(path)
        self.root_path: pathlib.Path = None

    def version_file_valid(self) -> bool:
        version_file = self.root_path / "VERSION"

        if version_file.is_file():
            with open(version_file, "r") as reader:
                version_string = reader.read()
            if re.match("^[0-9]+\.[0-9]+$", version_string):
                return True
        return False

    def readme_file_valid(self) -> bool:
        readme_file = self.root_path / "README.md"

        if readme_file.is_file():
            with open(readme_file, "r") as reader:
                readme_string = reader.read(500)
            if re.search("Machinekit-HAL", readme_string):
                return True
        return False

    def copying_file_valid(self) -> bool:
        copying_file = self.root_path / "COPYING"

        if copying_file.is_file():
            return True
        return False

    def is_machinekit_hal_repository_root(self) -> bool:
        if (
            self.version_file_valid()
            and self.readme_file_valid()
            and self.copying_file_valid()
        ):
            return True
        return False

    def getGitRepositoryRoot(self) -> None:
        try:
            self.root_path = pathlib.Path(sh.git("rev-parse",
                                                 "--show-toplevel",
                                                 _tty_out=False,
                                                 _cwd=self.path).strip().rstrip(os.sep))
        except sh.ErrorReturnCode as e:
            error_message = "Path {} is not a git repository. Error {}".format(
                self.path, e)
            raise ValueError(error_message)

    def __call__(self) -> pathlib.Path:
        self.getGitRepositoryRoot()
        if self.is_machinekit_hal_repository_root():
            return self.root_path
        else:
            error_message = "Path {} is not a Machinekit-HAL repository.".format(
                self.path)
            raise ValueError(error_message)
