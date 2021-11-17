# ~~~
# ####################################################################
# Description:  PEP503PythonIndex.cmake
#
#               This file, 'PEP503PythonIndex.cmake', implements build system
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

if(NOT DEFINED MACHINEKIT_HAL_PYTHON_INDEX)
  message(
    FATAL_ERROR
      "Variable MACHINEKIT_HAL_PYTHON_INDEX is not defined (and it has to be)!")
endif()

set(PYTHON_VENV_DIRECTORY
    "${MACHINEKIT_HAL_ARTIFACTS_MOUNTPOINT_DIRECTORY}/venv")

find_package(
  Python
  COMPONENTS Interpreter
  REQUIRED)

find_program(FIND "find" REQUIRED)

add_custom_target(
  create_binary_tree_venv ALL
  COMMAND ${CMAKE_COMMAND} "-E" "rm" "-rf" "${PYTHON_VENV_DIRECTORY}"
  COMMAND ${CMAKE_COMMAND} "-E" "make_directory" "${PYTHON_VENV_DIRECTORY}"
  COMMAND Python::Interpreter "-m" "venv" "--system-site-packages" "--prompt"
          "machinekit-hal" "${PYTHON_VENV_DIRECTORY}"
  COMMENT
    "(Re)Creating the BINARY tree runtime ${Python_INTERPRETER_ID} ${Python_VERSION} virtual environment"
)

add_custom_target(
  generate_pep503_index
  COMMAND Python::Interpreter
          "${CMAKE_CURRENT_BINARY_DIR}/create_pep503_index.py"
  COMMENT "Generating the PEP503 Index repository"
  VERBATIM)
