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
  option(BUILD_PCA9685_LIBRARY "Built the PCA9685 library Python distribution."
         TRUE)

  if(BUILD_PCA9685_LIBRARY)
    find_program(PYPROJECT_BUILD "pyproject-build" REQUIRED)
    include(PEP427Installer)

    set(PACKAGE_NAME "pca9685")

    set(DISTRIBUTION_NAME "${MACHINEKIT_HAL_PACKAGE_PREFIX}_${PACKAGE_NAME}")
    string(REGEX REPLACE "[._~ ]" "-" DISTRIBUTION_NORMALIZED_NAME
                         ${DISTRIBUTION_NAME})

    # Files representing the Python package
    set(PCA9685_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/src/PCA9685.py)
    set(PYTHON_INIT ${CMAKE_CURRENT_SOURCE_DIR}/src/__init__.py)
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
    configure_file(
      ${PCA9685_SOURCE}
      ${PACKAGE_NAME}/src/${MACHINEKIT_HAL_PACKAGE_PREFIX_PATH}/${PACKAGE_NAME}/PCA9685.py
      COPYONLY
      NO_SOURCE_PERMISSIONS)
    configure_file(${PYTHON_PYPROJECT} ${PACKAGE_NAME}/pyproject.toml @ONLY
                   NEWLINE_STYLE UNIX NO_SOURCE_PERMISSIONS)

    add_custom_command(
      OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/build_${PACKAGE_NAME}_wheel.stamp
      DEPENDS
        ${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}/src/${MACHINEKIT_HAL_PACKAGE_PREFIX_PATH}/${PACKAGE_NAME}/__init__.py
        ${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}/src/${MACHINEKIT_HAL_PACKAGE_PREFIX_PATH}/${PACKAGE_NAME}/PCA9685.py
        ${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}/pyproject.toml
      COMMAND ${CMAKE_COMMAND} "-E" "rm" "-f" "${OUTPUT_INDEX}/*.whl"
      COMMAND ${CMAKE_COMMAND} "-E" "make_directory" "${OUTPUT_INDEX}"
      COMMAND
        ${CMAKE_COMMAND} "-E" "env"
        "CMAKE_PREFIX_PATH=${MACHINEKIT_HAL_CMAKE_PACKAGE_OUTPUT_DIRECTORY}"
        "CMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}" "${PYPROJECT_BUILD}" "-w"
        "-o" "${OUTPUT_INDEX}" "${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}"
      COMMAND ${CMAKE_COMMAND} "-E" "touch"
              "${CMAKE_CURRENT_BINARY_DIR}/build_${PACKAGE_NAME}_wheel.stamp"
      COMMENT
        "Building Python package wheel ${DISTRIBUTION_NAME} via external tool")

    add_custom_command(
      OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/build_${PACKAGE_NAME}_sdist.stamp
      DEPENDS
        ${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}/src/${MACHINEKIT_HAL_PACKAGE_PREFIX_PATH}/${PACKAGE_NAME}/__init__.py
        ${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}/src/${MACHINEKIT_HAL_PACKAGE_PREFIX_PATH}/${PACKAGE_NAME}/PCA9685.py
        ${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}/pyproject.toml
      COMMAND ${CMAKE_COMMAND} "-E" "rm" "-f" "${OUTPUT_INDEX}/*.tar.gz"
      COMMAND ${CMAKE_COMMAND} "-E" "make_directory" "${OUTPUT_INDEX}"
      COMMAND
        ${CMAKE_COMMAND} "-E" "env"
        "CMAKE_PREFIX_PATH=${MACHINEKIT_HAL_CMAKE_PACKAGE_OUTPUT_DIRECTORY}"
        "CMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}" "${PYPROJECT_BUILD}" "-s"
        "-o" "${OUTPUT_INDEX}" "${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}"
      COMMAND ${CMAKE_COMMAND} "-E" "touch"
              "${CMAKE_CURRENT_BINARY_DIR}/build_${PACKAGE_NAME}_sdist.stamp"
      COMMENT
        "Building Python package sdist ${DISTRIBUTION_NAME} via external tool")

    add_custom_target(
      ${DISTRIBUTION_NAME}_wheel_distribution ALL
      DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/build_${PACKAGE_NAME}_wheel.stamp
      COMMENT
        "Requesting building Python package wheel ${DISTRIBUTION_NAME} via external tool"
    )

    add_custom_target(
      ${DISTRIBUTION_NAME}_sdist_distribution ALL
      DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/build_${PACKAGE_NAME}_sdist.stamp
      COMMENT
        "Requesting building Python package sdist ${DISTRIBUTION_NAME} via external tool"
    )

    install_wheel(WHEEL "${OUTPUT_INDEX}" COMPONENT
                  MachinekitHAL_Library_PCA9685_Python_Packages)

    cpack_add_component(MachinekitHAL_Library_PCA9685_Python_Packages
                        GROUP MachinekitHAL_Library_PCA9685)

    # Specification of artifacts placement in package tree
    cpack_add_component_group(
      MachinekitHAL_Library_PCA9685
      PARENT_GROUP MachinekitHAL_Package_Base_Python_Libraries)
  endif()
endif()
