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

ext_modules = [
    cmake_build_extension.CMakeExtension(name='pyhal',
                                         install_prefix="machinekit/hal/pyhal",
                                         source_dir=str(
                                              pathlib.Path(__file__).absolute().parent),
                                         cmake_configure_options=[
                                             f"-DPython3_ROOT_DIR={pathlib.Path(sys.prefix)}",
                                             "-DCALL_FROM_SETUP_PY:BOOL=TRUE",
                                             "-DBUILD_SHARED_LIBS:BOOL=FALSE",
                                         ] + CMAKE_CONFIGURE_EXTENSION_OPTIONS
                                         )
]

cmdclass = dict(build_ext=cmake_build_extension.BuildExtension)


def build(setup_kwargs, **kwargs):
    setup_kwargs.update(ext_modules=ext_modules,
                        cmdclass=cmdclass)
