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
  option(BUILD_LINUX_EVENT_LIBRARY
         "Built the Linux_event library Python distribution." TRUE)

  if(BUILD_LINUX_EVENT_LIBRARY)
    find_program(PYPROJECT_BUILD "pyproject-build" REQUIRED)

    set(PACKAGE_NAME "linux_event")

    set(DISTRIBUTION_NAME "${MACHINEKIT_HAL_PACKAGE_PREFIX}_${PACKAGE_NAME}")
    string(REGEX REPLACE "[._~ ]" "-" DISTRIBUTION_NORMALIZED_NAME
                         ${DISTRIBUTION_NAME})

    # Files representing the Python package
    set(LINUX_EVENT_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/src/linux_event.py)
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
      ${LINUX_EVENT_SOURCE}
      ${PACKAGE_NAME}/src/${MACHINEKIT_HAL_PACKAGE_PREFIX_PATH}/${PACKAGE_NAME}/linux_event.py
      COPYONLY
      NO_SOURCE_PERMISSIONS)
    configure_file(${PYTHON_PYPROJECT} ${PACKAGE_NAME}/pyproject.toml @ONLY
                   NEWLINE_STYLE UNIX NO_SOURCE_PERMISSIONS)

    add_custom_target(
      ${DISTRIBUTION_NAME}_distribution ALL
      DEPENDS
        ${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}/src/${MACHINEKIT_HAL_PACKAGE_PREFIX_PATH}/${PACKAGE_NAME}/__init__.py
        ${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}/src/${MACHINEKIT_HAL_PACKAGE_PREFIX_PATH}/${PACKAGE_NAME}/linux_event.py
        ${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}/pyproject.toml
      COMMAND ${CMAKE_COMMAND} "-E" "rm" "-rf" "${OUTPUT_INDEX}"
      COMMAND ${CMAKE_COMMAND} "-E" "make_directory" "${OUTPUT_INDEX}"
      COMMAND ${PYPROJECT_BUILD} -s -w -o ${OUTPUT_INDEX}
              ${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}
      COMMENT "Building Python package ${DISTRIBUTION_NAME} via external tool")
  endif()
endif()
