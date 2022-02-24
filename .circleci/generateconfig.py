#!/bin/env python3
import argparse
import pathlib
import json
import os

from ruamel.yaml import YAML

import importlib.util
spec = importlib.util.spec_from_file_location(
    "machinekit_hal_script_helpers",
    "{0}/support/python/machinekit_hal_script_helpers.py".format(
        os.path.realpath(
            "{0}/..".format(os.path.dirname(os.path.abspath(__file__)))
        )))
helpers = importlib.util.module_from_spec(spec)
spec.loader.exec_module(helpers)


class MachinekitHALCircleCIGenerator:
    TEST_ARCHITECTURES_TO_EXECUTORS = {
        'amd64': 'basic-amd64',
        'arm64': 'basic-arm64',
    }

    DEBIAN_DISTRO_SETTINGS = pathlib.Path(
        'debian/buildsystem/debian-distro-settings.json'
    )

    CIRCLE_CI_BASE = pathlib.Path('.circleci/base.yaml')

    def __init__(self, generate_config_path: pathlib.Path, test_architectures: list):
        """The MachinekitHALCircleCIGenerator's contructor. The execution is dependent
        on the '.circleci/base.yaml' and 'debian/buildsystem/debian-distro-settings.json'
        files and will error out of these files do no contain valid information
        or are in othervise invalid state (for example use unknown schema)

        Args:
            generate_config_path (pathlib.Path): Valid path to as of yet non-existing file
                                                 where the Circle CI configuration will be
                                                 generated
            test_architectures (list): Real hardware executor architectures on which the
                                       compilation and testing will be performed

        Raises:
            RuntimeError: Raised when the passed architecture arguments are incorrect
        """
        self.output_path = generate_config_path

        if not isinstance(generate_config_path, pathlib.Path):
            generate_config_path = pathlib.Path(generate_config_path)
        if not isinstance(test_architectures, list):
            test_architectures = list(test_architectures)

        if not set(test_architectures).issubset(set(self.TEST_ARCHITECTURES_TO_EXECUTORS.keys())):
            raise RuntimeError(
                f'Disallowed testing architectures {test_architectures}')
        self.test_architectures = test_architectures

        repository_root = helpers.NormalizeMachinekitHALPath(__file__)()

        configuration_file = repository_root / self.DEBIAN_DISTRO_SETTINGS
        circleci_base_file = repository_root / self.CIRCLE_CI_BASE

        self.yaml = YAML(typ='safe')
        self.yaml.default_flow_style = False

        with open(circleci_base_file, 'r') as file:
            self.circleci_base = self.yaml.load(file)

        with open(configuration_file, 'r') as file:
            configurations = json.load(file)

        self.allowed_configurations = configurations.get(
            'allowedCombinations', list())
        self.os_versions = configurations.get('osVersions', list())

        self.package_build_configurations = list()
        self.package_test_configurations = list()
        self.build_tree_test_suites_configurations = list()

    def generate_builder_configurations(self):
        """Generate the build and testing configuration for Circle CI workflow

        Raises:
            RuntimeError: Raised when the input data from 'debian/buildsystem/ \
                          debian-distro-settings.json' is not valid
        """
        os_lut = dict()
        self.build_configurations = list()

        for os in self.os_versions:
            release_number = os.get('releaseNumber', None)

            if release_number is None:
                raise RuntimeError(
                    f'Invalid settings: Release Number: {release_number} in {os}')

            os_lut[release_number] = os

        for configuration in self.allowed_configurations:
            target_architecture = configuration.get('architecture', None)
            os_version_number = configuration.get('osVersionNumber', None)

            if not (target_architecture and os_version_number):
                raise RuntimeError(f'Invalid settings: Architecture: {target_architecture}, '
                                   f'osVersionNumber: {os_version_number} in {configuration}')

            distribution_name = os_lut.get(os_version_number, None).get(
                'distributionID', None)
            distribution_codename = os_lut.get(
                os_version_number, None).get('distributionCodename', None)

            if not (distribution_name and distribution_codename):
                raise RuntimeError(f'Invalid settings: Distribution: {distribution_name}, '
                                   f'Codename: {distribution_codename} in {os_lut}')

            distribution_name = distribution_name.lower()
            distribution_version = str(os_version_number).lower()
            distribution_codename = distribution_codename.lower()
            target_architecture = target_architecture.lower()

            for architecture in self.test_architectures:
                executor = self.TEST_ARCHITECTURES_TO_EXECUTORS[architecture]

                self.package_build_configurations.append(
                    {
                        'buildMachinekitHALPackages': dict(
                            name=f'buildMachinekitHALPackages-{distribution_name}-{distribution_codename}-{target_architecture}',
                            distribution_name=distribution_name,
                            distribution_version=distribution_version,
                            target_architecture=target_architecture,
                            executor=executor,
                        )
                    }
                )

                if architecture == target_architecture:
                    # Jobs which will run the Runtest testsuite installed from Deb packages in
                    # Docker containers
                    self.package_test_configurations.append(
                        {
                            'testMachinekitHALPackages': dict(
                                name=f'testMachinekitHALPackages-{distribution_name}-{distribution_codename}-{target_architecture}',
                                requires=[
                                    f'buildMachinekitHALPackages-{distribution_name}-{distribution_codename}-{target_architecture}'],
                                distribution_name=distribution_name,
                                distribution_version=distribution_version,
                                target_architecture=target_architecture,
                                executor=executor,
                            )
                        }
                    )

                    # Jobs which will run the testsuites (PyTests and Runtests)
                    # from the CMake build tree
                    self.build_tree_test_suites_configurations.append(
                        {
                            'buildDebianVersionAndRunTestsuites': dict(
                                name=f'buildDebianVersionAndRunTestsuites-{distribution_name}-{distribution_codename}-{target_architecture}',
                                distribution_name=distribution_name,
                                distribution_version=distribution_version,
                                target_architecture=target_architecture,
                                executor=executor,
                            )
                        }
                    )

    def generate_circle_ci_config(self):
        """Writes the henerated Circle CI workflow configuration to the YAML file
        path specified in the constructor
        """
        configuration = dict(self.circleci_base)
        workflow = {
            'workflows': {
                'buildMachinekitHAL': {
                    'jobs': self.package_build_configurations +
                    self.package_test_configurations +
                    self.build_tree_test_suites_configurations
                }
            }
        }

        final = configuration | workflow

        with open(self.output_path, 'w') as file:
            self.yaml.dump(final, file)


def main(args):
    generator = MachinekitHALCircleCIGenerator(
        args.output_path, args.test_architectures
    )

    generator.generate_builder_configurations()
    generator.generate_circle_ci_config()


class StoreValidYAMLPath(argparse.Action):
    """Ad-hoc Argparser Action to check validity of YAML path specification
    and create needed directory structure in advance
    """

    def test_path(self, path: str) -> pathlib.Path:
        try:
            yaml_path = pathlib.Path(path).resolve(strict=False)

            if yaml_path.suffix not in ['.yaml', '.yml']:
                raise argparse.ArgumentError(self,
                                             f'Path {path} does not have valid YAML extension')

            yaml_path.parent.mkdir(parents=True, exist_ok=True)
            if yaml_path.exists():
                raise argparse.ArgumentError(self,
                                             f'Path {yaml_path} already exists. Not going to overwite!')
            return yaml_path
        except Exception as e:
            raise argparse.ArgumentError(self,
                                         f'Path {path} checking caused unknown error: {e}')

        if not os.path.isdir(path):
            raise argparse.ArgumentError(self,
                                         "Path {} does not exist.".format(path))
        if not os.access(path, os.W_OK):
            raise argparse.ArgumentError(self,
                                         "Path {} cannot be written to.".format(path))
        return os.path.realpath(os.path.abspath(path.rstrip(os.sep)))

    def __call__(self, parser, namespace, values, option_string=None):
        if type(values) == list:
            paths = map(self.test_path, values)
        else:
            paths = self.test_path(values)

        setattr(namespace, self.dest, paths)


if __name__ == "__main__":
    """ This is executed when run from the command line """
    parser = argparse.ArgumentParser(
        description="Dynamic configuration tool for Circle CI Machinekit-HAL")

    # Mandatory argument for path to Machinekit-HAL repository
    parser.add_argument(action=StoreValidYAMLPath,
                        dest='output_path',
                        metavar='PATH',
                        help="Path of YAML file with generated configuration for Circle CI")
    # Voluntarily mandatory argument for selecting architectures which will be tested
    parser.add_argument('-a',
                        '--architecture',
                        action='store',
                        dest='test_architectures',
                        nargs='*',
                        metavar='ARCHITECTURE',
                        choices=MachinekitHALCircleCIGenerator.TEST_ARCHITECTURES_TO_EXECUTORS.keys(),
                        type=str.lower,
                        default=['arm64'],
                        help="Path of YAML file with generated configuration for Circle CI")

    args = parser.parse_args()

    main(args)
