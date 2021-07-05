#!/usr/bin/env python3
# -*- coding: utf-8 -*-

#####################################################################
# Description:  configure.py
#
#               This file, 'configure.py', implements higher functions which
#               are needed for building packages for Debian like distributions
#               It is intended as logical continuation to 'debian/bootsrap'
#               bash script with difference that this script can use all
#               build-time dependencies specified in 'debian/control.in'
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
Script for configuring packaging process for Debian styled distributions using
'dpkg' tool and .deb or .ddeb suffixed packages
"""

# Debian 9 Stretch, Ubuntu 18.04 Bionic and (probably) other older distributions
# need in environment LANG=C.UTF-8 (or other similar specification of encoding)
# to properly function

__license__ = "LGPL 2.1"

import argparse
import sh
import os
import re
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


class Configure_script():
    def __init__(self: object, path):
        self.normalized_path = helpers.NormalizeMachinekitHALPath(path)()
        self.get_machinekit_hal_changelog_data()
        self.machinekit_hal_version_string = "{0}.{1}.{2}-1.git{3}~{4}".format(
            self.major_version,
            self.minor_version,
            self.commit_count,
            self.git_sha[0:9],
            self.distro_codename)
        self.machinekit_hal_tarball_string = "{0}.{1}.{2}".format(
            self.major_version,
            self.minor_version,
            self.commit_count,
        )

    def get_machinekit_hal_changelog_data(self: object) -> None:
        control_template_file = "{}/debian/control.in".format(
            self.normalized_path)
        with open(control_template_file, "r") as reader:
            control_template_string = reader.read()
        self.name = re.search(
            "Source: (.+)\n", control_template_string).group(1).strip().lower()
        version_file = "{}/VERSION".format(self.normalized_path)
        with open(version_file, "r") as reader:
            version_string = reader.read()
        base_version = version_string.strip().split('.')
        self.major_version = base_version[0]
        self.minor_version = base_version[1]
        self.distro_id = sh.lsb_release("-is",
                                        _tty_out=False).strip().replace(" ", "").lower()
        self.distro_codename = sh.lsb_release("-cs",
                                              _tty_out=False).strip().replace(" ", "").lower()
        self.git_sha = sh.git("rev-parse",
                              "HEAD",
                              _tty_out=False,
                              _cwd=self.normalized_path).strip()
        self.commit_count = int(sh.git("rev-list",
                                       "--count",
                                       "HEAD",
                                       _tty_out=False,
                                       _cwd=self.normalized_path).strip())
        self.author_name = sh.git("show",
                                  "-s",
                                  "--pretty=%an",
                                  "HEAD",
                                  _tty_out=False,
                                  _cwd=self.normalized_path).rstrip('\n')
        self.author_email = sh.git("show",
                                   "-s",
                                   "--format=%ae",
                                   "HEAD",
                                   _tty_out=False,
                                   _cwd=self.normalized_path).strip()
        self.commit_message_short = sh.git("show",
                                           "-s",
                                           "--format=format:%s",
                                           "HEAD",
                                           _tty_out=False,
                                           _cwd=self.normalized_path).strip()
        self.commit_time = sh.git("show",
                                  "-s",
                                  "--format=format:%aD",
                                  "HEAD",
                                  _tty_out=False,
                                  _cwd=self.normalized_path).strip()
        self.debian_host_architecture = sh.dpkg_architecture("-qDEB_HOST_ARCH",
                                                             _tty_out=False).strip()
        self.debian_build_architecture = sh.dpkg_architecture("-qDEB_BUILD_ARCH",
                                                              _tty_out=False).strip()

    def get_machinekit_hal_base_changelog(self: object) -> tuple:
        changelog_template_file = "{}/debian/changelog.in".format(
            self.normalized_path)
        changelog_file = "{}/debian/changelog".format(
            self.normalized_path)
        if os.path.isfile(changelog_file):
            file = changelog_file
        else:
            file = changelog_template_file
        with open(file, "r") as reader:
            old_changelog_string = reader.read()
        versions = re.findall("^\S+ \((.+)\) \w+; \w+=\w+\n",
                              old_changelog_string,
                              re.MULTILINE)
        return (old_changelog_string, versions)

    def write_machinekit_hal_base_changelog(self: object, changelog_text) -> None:
        changelog_file = "{}/debian/changelog".format(
            self.normalized_path)
        with open(changelog_file, "w") as writer:
            writer.write(changelog_text)

    def create_changelog(self: object) -> None:
        new_item = """{0} ({1}) {2}; urgency=low

  * Commit: {3}
  * {4}
  * Build for {5} {6} with DEB_BUILD_ARCH {7} and DEB_HOST_ARCH {8}
    as an {9}th rebuild

 -- {10} <{11}>  {12}\n""".format(self.name,
                                  self.machinekit_hal_version_string,
                                  self.distro_codename,
                                  self.git_sha,
                                  self.commit_message_short,
                                  self.distro_id.capitalize(),
                                  self.distro_codename.capitalize(),
                                  self.debian_build_architecture,
                                  self.debian_host_architecture,
                                  self.commit_count,
                                  self.author_name,
                                  self.author_email,
                                  self.commit_time)
        machinekit_hal_changelog = self.get_machinekit_hal_base_changelog()
        if self.machinekit_hal_version_string not in machinekit_hal_changelog[1]:
            new_changelog = "{}\n{}".format(
                new_item, machinekit_hal_changelog[0])
            self.write_machinekit_hal_base_changelog(new_changelog)
        else:
            print("Machinekit-HAL chnagelog entry with version {0} already exists!".format(
                self.machinekit_hal_version_string))

    def create_tarball(self: object, output_path) -> None:
        tarball_file = "{0}/machinekit-hal_{1}.orig.tar.gz".format(
            output_path, self.machinekit_hal_tarball_string)
        sh.git("archive",
               "--format=tar.gz",
               "-o",
               tarball_file,
               "HEAD",
               _tty_out=False,
               _cwd=self.normalized_path)


def main(args):
    """ Main entry point of the app """

    if args.changelog is False and args.tarball is None:
        raise ValueError(
            "At least one of --changelog or --tarball has to be specified.")
    try:
        configure_script = Configure_script(args.path)
        if args.changelog:
            configure_script.create_changelog()
            print("Debian changelog created!")
        if args.tarball:
            configure_script.create_tarball(args.tarball)
            print("Debian source tarball created!")
        print("Configure tool ran successfully to completion!")
    except ValueError as e:
        print(e)
        sys.exit(1)


if __name__ == "__main__":
    """ This is executed when run from the command line """
    parser = argparse.ArgumentParser(
        description="Configure packaging for Debian like distributions")

    # Optional argument for path to Machinekit-HAL repository
    parser.add_argument("-p",
                        "--path",
                        action=helpers.PathExistsAction,
                        dest="path",
                        default=os.getcwd(),
                        help="Path to root of Machinekit-HAL repository")
    # Optional argument for changelog creation
    parser.add_argument("-c",
                        "--changelog",
                        dest="changelog",
                        action="store_true",
                        help="Create Debian changelog with automatic values")
    # Optional argument for tarball creation
    parser.add_argument("-t",
                        "--tarball",
                        action=helpers.PathExistsAction,
                        dest="tarball",
                        metavar="PATH",
                        help="Create Debian source tarball for non-binary package build in PATH")

    args = parser.parse_args()

    main(args)
