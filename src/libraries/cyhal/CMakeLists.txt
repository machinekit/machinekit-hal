# ~~~
# ####################################################################
# Description:  CMakeLists.txt
#
#               This file, 'CMakeLists.txt', implements build system
#               rules for Machinekit-HAL project
#
# Copyright (C) 2021    Jakub Fišer  <jakub DOT fiser AT eryaf DOT com>
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
# #####################################################################
# ~~~

if(BUILD_PYTHON_DISTRIBUTIONS)
  option(BUILD_CYHAL_MODULE_LIBRARY
         "Built the CyHAL Python Cython extension library distribution." TRUE)

  if(BUILD_CYHAL_MODULE_LIBRARY)
    find_program(PYPROJECT_BUILD "pyproject-build" REQUIRED)
    find_program(CYTHON "cython" "cython3" REQUIRED)
    include(PEP427Installer)

    set(PACKAGE_NAME "cyhal")

    set(DISTRIBUTION_NAME "${MACHINEKIT_HAL_PACKAGE_PREFIX}_${PACKAGE_NAME}")
    string(REGEX REPLACE "[._~ ]" "-" DISTRIBUTION_NORMALIZED_NAME
                         ${DISTRIBUTION_NAME})

    # Files representing the Python package
    set(CYHAL_MODULE_MAIN_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/src/hal.pyx)

    set(CYHAL_MODULE_DEPENDENCY_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/hal_component.pyx
        ${CMAKE_CURRENT_SOURCE_DIR}/src/hal_const.pxd
        ${CMAKE_CURRENT_SOURCE_DIR}/src/hal_data.pyx
        ${CMAKE_CURRENT_SOURCE_DIR}/src/hal_epsilon.pyx
        ${CMAKE_CURRENT_SOURCE_DIR}/src/hal_funct.pyx
        ${CMAKE_CURRENT_SOURCE_DIR}/src/hal_group.pxd
        ${CMAKE_CURRENT_SOURCE_DIR}/src/hal_group.pyx
        ${CMAKE_CURRENT_SOURCE_DIR}/src/hal_instdict.pyx
        ${CMAKE_CURRENT_SOURCE_DIR}/src/hal_inst.pyx
        ${CMAKE_CURRENT_SOURCE_DIR}/src/hal_iter.pxd
        ${CMAKE_CURRENT_SOURCE_DIR}/src/hal_loadusr.pyx
        ${CMAKE_CURRENT_SOURCE_DIR}/src/hal_net.pyx
        ${CMAKE_CURRENT_SOURCE_DIR}/src/hal_objectdict.pyx
        ${CMAKE_CURRENT_SOURCE_DIR}/src/hal_objectops.pxd
        ${CMAKE_CURRENT_SOURCE_DIR}/src/hal_object.pyx
        ${CMAKE_CURRENT_SOURCE_DIR}/src/hal_pindict.pyx
        ${CMAKE_CURRENT_SOURCE_DIR}/src/hal_pin.pyx
        ${CMAKE_CURRENT_SOURCE_DIR}/src/hal_priv.pxd
        ${CMAKE_CURRENT_SOURCE_DIR}/src/hal.pxd
        ${CMAKE_CURRENT_SOURCE_DIR}/src/hal_rcomp.pxd
        ${CMAKE_CURRENT_SOURCE_DIR}/src/hal_rcomp.pyx
        ${CMAKE_CURRENT_SOURCE_DIR}/src/hal_ring.pxd
        ${CMAKE_CURRENT_SOURCE_DIR}/src/hal_ring.pyx
        ${CMAKE_CURRENT_SOURCE_DIR}/src/hal_sigdict.pyx
        ${CMAKE_CURRENT_SOURCE_DIR}/src/hal_signal.pyx
        ${CMAKE_CURRENT_SOURCE_DIR}/src/hal_threads.pyx
        ${CMAKE_CURRENT_SOURCE_DIR}/src/hal_util.pxd
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ring_const.pxd
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ring.pxd)

    # TODO: This is ugly, set it so it does not mention wheel or sdist
    set(PRIVATE_HEADER_FILES
        "$<TARGET_PROPERTY:machinekit_hal_cyruntime_wheel_distribution,PUBLIC_INCLUDE_DIRECTORY>"
    )

    set(PYTHON_INIT ${CMAKE_CURRENT_SOURCE_DIR}/src/__init__.py)
    set(PYTHON_PDM_BUILD ${CMAKE_CURRENT_SOURCE_DIR}/src/build.py)
    set(CYHAL_MODULE_CMAKELISTS
        ${CMAKE_CURRENT_SOURCE_DIR}/src/CMakeLists.txt.in)
    set(PYTHON_PYPROJECT ${CMAKE_CURRENT_SOURCE_DIR}/src/pyproject.toml.in)

    set(OUTPUT_INDEX
        ${MACHINEKIT_HAL_PYTHON_INDEX}/${DISTRIBUTION_NORMALIZED_NAME})

    file(REMOVE_RECURSE
         ${CMAKE_CURRENT_BINARY_DIR}/${DISTRIBUTION_NORMALIZED_NAME})
    configure_file(
      ${PYTHON_INIT}
      ${PACKAGE_NAME}/src/${MACHINEKIT_HAL_PACKAGE_PREFIX_PATH}/${PACKAGE_NAME}/__init__.py
      COPYONLY
      NO_SOURCE_PERMISSIONS)
    configure_file(${PYTHON_PDM_BUILD} ${PACKAGE_NAME}/build.py COPYONLY
                   NO_SOURCE_PERMISSIONS)
    configure_file(${CYHAL_MODULE_CMAKELISTS} ${PACKAGE_NAME}/CMakeLists.txt
                   @ONLY NEWLINE_STYLE UNIX NO_SOURCE_PERMISSIONS)
    configure_file(${PYTHON_PYPROJECT} ${PACKAGE_NAME}/pyproject.toml @ONLY
                   NEWLINE_STYLE UNIX NO_SOURCE_PERMISSIONS)

    add_custom_command(
      OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}/src/${MACHINEKIT_HAL_PACKAGE_PREFIX_PATH}/${PACKAGE_NAME}/hal.c
      DEPENDS ${CYHAL_MODULE_MAIN_SOURCE} ${CYHAL_MODULE_DEPENDENCY_SOURCES}
              ${PRIVATE_HEADER_FILES} ${PRIVATE_HEADER_FILES}
      COMMAND
        ${CYTHON} "-3" "-w" "${CMAKE_CURRENT_BINARY_DIR}" "-I"
        "${CMAKE_CURRENT_SOURCE_DIR}/src" "-I"
        "$<TARGET_PROPERTY:runtime_api,PUBLIC_INCLUDE_DIRECTORY>" "-I"
        "${PRIVATE_HEADER_FILES}" "-I"
        "$<TARGET_PROPERTY:hal_api,PUBLIC_INCLUDE_DIRECTORY>" "-o"
        "${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}/src/${MACHINEKIT_HAL_PACKAGE_PREFIX_PATH}/${PACKAGE_NAME}/hal.c"
        "-t" "${CYHAL_MODULE_MAIN_SOURCE}"
      COMMENT
        "Translating the Cython code for Python package ${DISTRIBUTION_NAME} via external tool"
    )

    if(PYTHON_PLAT_NAME)
      set(_python_plat_name
          "--config-setting=\"--plat-name=${PYTHON_PLAT_NAME}\"")
    else()
      set(_python_plat_name "") # Make sure it is empty
    endif()

    add_custom_command(
      OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/build_${PACKAGE_NAME}_wheel.stamp
      DEPENDS
        ${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}/src/${MACHINEKIT_HAL_PACKAGE_PREFIX_PATH}/${PACKAGE_NAME}/__init__.py
        ${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}/src/${MACHINEKIT_HAL_PACKAGE_PREFIX_PATH}/${PACKAGE_NAME}/hal.c
        ${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}/pyproject.toml
        ${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}/CMakeLists.txt
      COMMAND ${CMAKE_COMMAND} "-E" "rm" "-f" "${OUTPUT_INDEX}/*.whl"
      COMMAND ${CMAKE_COMMAND} "-E" "make_directory" "${OUTPUT_INDEX}"
      # TODO: This is hackins approach to solve issue with building multiple
      # CONFIGS: Each config build is separated and because the wheel build uses
      # an isolated environment, the paths to executables (python, pip etc.) are
      # stored in CMake cache during generation. Second (et al) builds then try
      # to reuse the first generation and fails because the temporary
      # environment from the first run no longer exists This will need a rework
      # as it should use its own manual isolation (for Debian and other distros
      # because they require it) and generally to keep the build files for
      # possibility of post-mortem
      COMMAND ${CMAKE_COMMAND} "-E" "rm" "-rf"
              "${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}/build"
      COMMAND
        ${CMAKE_COMMAND} "-E" "env"
        "CMAKE_PREFIX_PATH=${MACHINEKIT_HAL_CMAKE_PACKAGE_OUTPUT_DIRECTORY}"
        "CMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}"
        "MACHINEKIT_HAL_BUILD_CONFIG=$<CONFIG>" "${PYPROJECT_BUILD}" "-w" "-o"
        "${OUTPUT_INDEX}" "${_python_plat_name}"
        "${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}"
      COMMAND ${CMAKE_COMMAND} "-E" "touch"
              "${CMAKE_CURRENT_BINARY_DIR}/build_${PACKAGE_NAME}_wheel.stamp"
      COMMENT
        "Building Python package wheel ${DISTRIBUTION_NAME} via external tool")

    add_custom_command(
      OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/build_${PACKAGE_NAME}_sdist.stamp
      DEPENDS
        ${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}/src/${MACHINEKIT_HAL_PACKAGE_PREFIX_PATH}/${PACKAGE_NAME}/__init__.py
        ${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}/src/${MACHINEKIT_HAL_PACKAGE_PREFIX_PATH}/${PACKAGE_NAME}/hal.c
        ${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}/pyproject.toml
        ${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}/CMakeLists.txt
      COMMAND ${CMAKE_COMMAND} "-E" "rm" "-f" "${OUTPUT_INDEX}/*.tar.gz"
      COMMAND ${CMAKE_COMMAND} "-E" "make_directory" "${OUTPUT_INDEX}"
      COMMAND
        ${CMAKE_COMMAND} "-E" "env"
        "CMAKE_PREFIX_PATH=${MACHINEKIT_HAL_CMAKE_PACKAGE_OUTPUT_DIRECTORY}"
        "CMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}"
        "MACHINEKIT_HAL_BUILD_CONFIG=$<CONFIG>" "${PYPROJECT_BUILD}" "-s" "-o"
        "${OUTPUT_INDEX}" "${_python_plat_name}"
        "${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}"
      COMMAND ${CMAKE_COMMAND} "-E" "touch"
              "${CMAKE_CURRENT_BINARY_DIR}/build_${PACKAGE_NAME}_sdist.stamp"
      COMMENT
        "Building Python package sdist ${DISTRIBUTION_NAME} via external tool")

    add_custom_target(
      ${DISTRIBUTION_NAME}_wheel_distribution ALL
      DEPENDS
        ${CMAKE_CURRENT_BINARY_DIR}/build_${PACKAGE_NAME}_wheel.stamp
        ${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}/src/${MACHINEKIT_HAL_PACKAGE_PREFIX_PATH}/${PACKAGE_NAME}/hal.c
      COMMENT
        "Requesting building Python package wheel ${DISTRIBUTION_NAME} via external tool"
    )

    add_custom_target(
      ${DISTRIBUTION_NAME}_sdist_distribution ALL
      DEPENDS
        ${CMAKE_CURRENT_BINARY_DIR}/build_${PACKAGE_NAME}_sdist.stamp
        ${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}/src/${MACHINEKIT_HAL_PACKAGE_PREFIX_PATH}/${PACKAGE_NAME}/hal.c
      COMMENT
        "Requesting building Python package sdist ${DISTRIBUTION_NAME} via external tool"
    )

    add_dependencies(${DISTRIBUTION_NAME}_wheel_distribution unmanaged_hal
                     unmanaged_runtime)
    add_dependencies(${DISTRIBUTION_NAME}_sdist_distribution unmanaged_hal
                     unmanaged_runtime)

    install_wheel(WHEEL "${OUTPUT_INDEX}" COMPONENT
                  MachinekitHAL_Library_CyHAL_Module_Python_Packages)

    cpack_add_component(MachinekitHAL_Library_CyHAL_Module_Python_Packages
                        GROUP MachinekitHAL_Library_CyHAL_Module)

    # Specification of artifacts placement in package tree
    cpack_add_component_group(
      MachinekitHAL_Library_CyHAL_Module
      PARENT_GROUP MachinekitHAL_Package_Base_Python_Libraries)
  endif()
endif()
