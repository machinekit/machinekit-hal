import pathlib
import sys
import os
import cmake_build_extension

CMAKE_CONFIGURE_EXTENSION_OPTIONS = []

if "CMAKE_MODULE_PATH" in os.environ:
    CMAKE_CONFIGURE_EXTENSION_OPTIONS.append(
        f"-DCMAKE_MODULE_PATH={os.environ['CMAKE_MODULE_PATH']}")

if "CMAKE_PREFIX_PATH" in os.environ:
    CMAKE_CONFIGURE_EXTENSION_OPTIONS.append(
        f"-DCMAKE_PREFIX_PATH={os.environ['CMAKE_PREFIX_PATH']}")

if "CMAKE_TOOLCHAIN_FILE" in os.environ and os.environ["CMAKE_TOOLCHAIN_FILE"]:
    CMAKE_CONFIGURE_EXTENSION_OPTIONS.append(
        f"--toolchain={os.environ['CMAKE_TOOLCHAIN_FILE']}")

ext_modules = [
    cmake_build_extension.CMakeExtension(name='cyhal',
                                         install_prefix="machinekit/hal/cyhal",
                                         source_dir=str(
                                              pathlib.Path(__file__).absolute().parent),
                                         cmake_build_type=os.environ.get(
                                             "MACHINEKIT_HAL_BUILD_CONFIG", "Release"),
                                         cmake_configure_options=[
                                             "-DBUILD_SHARED_LIBS:BOOL=FALSE",
                                         ] + CMAKE_CONFIGURE_EXTENSION_OPTIONS
                                         )
]

cmdclass = dict(build_ext=cmake_build_extension.BuildExtension)


def build(setup_kwargs, **kwargs):
    setup_kwargs.update(ext_modules=ext_modules,
                        cmdclass=cmdclass)
