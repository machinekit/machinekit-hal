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
  find_package(
    Python
    COMPONENTS Interpreter
    REQUIRED)
  option(BUILD_INSTCOMP_EXECUTABLE
         "Built the Instcomp executable Python distribution." TRUE)

  if(BUILD_INSTCOMP_EXECUTABLE)
    find_program(YAPPS2 "yapps2" REQUIRED)
    find_program(PYPROJECT_BUILD "pyproject-build" REQUIRED)
    include(PEP427Installer)

    define_property(
      GLOBAL
      PROPERTY MACHINEKIT_HAL_INSTCOMP
      BRIEF_DOCS "Path to the Instcomp executable"
      FULL_DOCS
        [[
      Machinekit-HAL buildsystem (internal only) global property to specify the Instcomp
      executable (which should live in a special Python virtual environment).
      ]])

    set(PACKAGE_NAME "instcomp")

    set(DISTRIBUTION_NAME "${MACHINEKIT_HAL_PACKAGE_PREFIX}_${PACKAGE_NAME}")
    string(REGEX REPLACE "[._~ ]" "-" DISTRIBUTION_NORMALIZED_NAME
                         ${DISTRIBUTION_NAME})

    # Files representing the Python package
    set(INSTCOMP_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/src/instcomp.g)
    set(PYTHON_INIT ${CMAKE_CURRENT_SOURCE_DIR}/src/__init__.py)
    set(PYTHON_PYPROJECT ${CMAKE_CURRENT_SOURCE_DIR}/src/pyproject.toml.in)

    set(OUTPUT_INDEX
        ${MACHINEKIT_HAL_PYTHON_INDEX}/${DISTRIBUTION_NORMALIZED_NAME})

    file(REMOVE_RECURSE
         ${CMAKE_CURRENT_BINARY_DIR}/${DISTRIBUTION_NORMALIZED_NAME})

    add_custom_command(
      OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}/src/${MACHINEKIT_HAL_PACKAGE_PREFIX_PATH}/${PACKAGE_NAME}/instcomp.py
      COMMAND
        ${YAPPS2} ${INSTCOMP_SOURCE}
        ${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}/src/${MACHINEKIT_HAL_PACKAGE_PREFIX_PATH}/${PACKAGE_NAME}/instcomp.py
      MAIN_DEPENDENCY ${INSTCOMP_SOURCE}
      COMMENT "Compiling the '${PACKAGE_NAME}' Python script")

    configure_file(
      src/__init__.py
      ${PACKAGE_NAME}/src/${MACHINEKIT_HAL_PACKAGE_PREFIX_PATH}/${PACKAGE_NAME}/__init__.py
      COPYONLY
      NO_SOURCE_PERMISSIONS)
    configure_file(src/pyproject.toml.in ${PACKAGE_NAME}/pyproject.toml @ONLY
                   NEWLINE_STYLE UNIX NO_SOURCE_PERMISSIONS)

    add_custom_command(
      OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/build_${PACKAGE_NAME}_wheel.stamp
      DEPENDS
        ${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}/src/${MACHINEKIT_HAL_PACKAGE_PREFIX_PATH}/${PACKAGE_NAME}/__init__.py
        ${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}/src/${MACHINEKIT_HAL_PACKAGE_PREFIX_PATH}/${PACKAGE_NAME}/instcomp.py
        ${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}/pyproject.toml
        ${INSTCOMP_SOURCE}
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
        ${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}/src/${MACHINEKIT_HAL_PACKAGE_PREFIX_PATH}/${PACKAGE_NAME}/instcomp.py
        ${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}/pyproject.toml
        ${INSTCOMP_SOURCE}
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
              ${INSTCOMP_SOURCE}
      COMMENT
        "Requesting building Python package wheel ${DISTRIBUTION_NAME} via external tool"
    )

    add_custom_target(
      ${DISTRIBUTION_NAME}_sdist_distribution ALL
      DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/build_${PACKAGE_NAME}_sdist.stamp
              ${INSTCOMP_SOURCE}
      COMMENT
        "Requesting building Python package sdist ${DISTRIBUTION_NAME} via external tool"
    )

    add_custom_command(
      OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/build_instcomp_executable.stamp
      DEPENDS ${DISTRIBUTION_NAME}_wheel_distribution
      COMMAND Python::Interpreter "-m" "venv"
              "${CMAKE_CURRENT_BINARY_DIR}/venv_instcomp"
      COMMAND
        ${CMAKE_COMMAND} "-E" "env"
        "${CMAKE_CURRENT_BINARY_DIR}/venv_instcomp/bin/pip" "install"
        "--force-reinstall" "${OUTPUT_INDEX}/*.whl"
      COMMENT "Creating virtual environment for the 'instcomp' tool")

    add_custom_target(
      instcomp_executable ALL
      DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/build_instcomp_executable.stamp
      COMMENT "Building the virtual environment for the 'instcomp' tool")

    add_dependencies(instcomp_executable
                     ${DISTRIBUTION_NAME}_wheel_distribution)

    configure_file(
      ${CMAKE_CURRENT_SOURCE_DIR}/src/MachinekitHALInstcompFunction.cmake
      ${MACHINEKIT_HAL_CMAKE_PACKAGE_OUTPUT_DIRECTORY}/MachinekitHALInstcompFunction.cmake
      @ONLY
      NEWLINE_STYLE UNIX NO_SOURCE_PERMISSIONS)

    set_property(
      GLOBAL PROPERTY MACHINEKIT_HAL_INSTCOMP
                      "${CMAKE_CURRENT_BINARY_DIR}/venv_instcomp/bin/instcomp")

    install_wheel(WHEEL "${OUTPUT_INDEX}" COMPONENT
                  MachinekitHAL_Executable_Instcomp_Python_Packages)

    cpack_add_component(
      MachinekitHAL_Executable_Instcomp_Python_Packages
      DEPENDS MachinekitHAL_Library_Symbol_Visibility_CMake_Functions
      GROUP MachinekitHAL_Executable_Instcomp)

    # Specification of artifacts placement in package tree
    cpack_add_component_group(
      MachinekitHAL_Executable_Instcomp
      PARENT_GROUP MachinekitHAL_Package_Base_Python_Executables)

  endif()
endif()
