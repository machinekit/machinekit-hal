# ~~~
# ####################################################################
# Description:  PEP427Installer.cmake
#
#               This file, 'PEP427Installer.cmake', implements build system
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

function(install_wheel)
  set(prefix "wheel_installer")
  set(noValues "")
  set(singleValues
      "WHEEL"
      "INTERPRETER"
      "STDLIB_PATH"
      "PLATSTDLIB_PATH"
      "PURELIB_PATH"
      "PLATLIB_PATH"
      "INCLUDE_PATH"
      "PLATINCLUDE_PATH"
      "COMPONENT")
  set(multiValues "")

  cmake_parse_arguments(PARSE_ARGV 0 "${prefix}" "${noValues}"
                        "${singleValues}" "${multiValues}")

  if(${prefix}_KEYWORDS_MISSING_VALUES)
    message(
      FATAL_ERROR
        "All keyword arguments need values! (${${prefix}_KEYWORDS_MISSING_VALUES})"
    )
  endif()

  list(APPEND all_key_arguments "${noValues}" "${singleValues}"
       "${multiValues}")
  if(NOT ${prefix}_WHEEL)
    message(
      FATAL_ERROR
        "Python wheel (file or parent directory) has to be specified! (WHEEL path)"
    )
  endif()

  cmake_path(IS_ABSOLUTE ${prefix}_WHEEL wheel_path_absolute)

  if(NOT wheel_path_absolute)
    message(FATAL_ERROR "Path '${${prefix}_WHEEL}' is NOT absolute!")
  endif()

  set(PYTHON_INSTALLER_SCRIPT
      "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/PEP427Installer/wheel_installer.py")
  set(installer_cmake_script_template
      "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/PEP427Installer/PythonWheelInstall.cmake.in"
  )

  if(${prefix}_INTERPRETER)
    set(PYTHON_INTERPRETER "${prefix}_INTERPRETER")
  else()
    set(PYTHON_INTERPRETER
        "${MACHINEKIT_HAL_PYTHON_INTERPRETER_INSTALL_EXECUTABLE}")
  endif()

  set(PYTHON_WHEEL_PACKAGE "${${prefix}_WHEEL}")

  if(${prefix}_STDLIB_PATH)
    set(PYTHON_STDLIB_PATH "\$ENV{DESTDIR}${${prefix}_STDLIB_PATH}")
  else()
    set(PYTHON_STDLIB_PATH
        "\$ENV{DESTDIR}${MACHINEKIT_HAL_PYTHON_STDLIB_FULL_INSTALL_DIRECTORY}")
  endif()
  if(${prefix}_PLATSTDLIB_PATH)
    set(PYTHON_PLATSTDLIB_PATH "\$ENV{DESTDIR}${${prefix}_PLATSTDLIB_PATH}")
  else()
    set(PYTHON_PLATSTDLIB_PATH
        "\$ENV{DESTDIR}${MACHINEKIT_HAL_PYTHON_PLATSTDLIB_FULL_INSTALL_DIRECTORY}"
    )
  endif()
  if(${prefix}_PURELIB_PATH)
    set(PYTHON_PURELIB_PATH "\$ENV{DESTDIR}${${prefix}_PURELIB_PATH}")
  else()
    set(PYTHON_PURELIB_PATH
        "\$ENV{DESTDIR}${MACHINEKIT_HAL_PYTHON_PURELIB_FULL_INSTALL_DIRECTORY}")
  endif()
  if(${prefix}_PLATLIB_PATH)
    set(PYTHON_PLATLIB_PATH "\$ENV{DESTDIR}${${prefix}_PLATLIB_PATH}")
  else()
    set(PYTHON_PLATLIB_PATH
        "\$ENV{DESTDIR}${MACHINEKIT_HAL_PYTHON_PLATLIB_FULL_INSTALL_DIRECTORY}")
  endif()
  if(${prefix}_INCLUDE_PATH)
    set(PYTHON_INCLUDE_PATH "\$ENV{DESTDIR}${${prefix}_INCLUDE_PATH}")
  else()
    set(PYTHON_INCLUDE_PATH
        "\$ENV{DESTDIR}${MACHINEKIT_HAL_PYTHON_INCLUDE_FULL_INSTALL_DIRECTORY}")
  endif()
  if(${prefix}_PLATINCLUDE_PATH)
    set(PYTHON_PLATINCLUDE_PATH "\$ENV{DESTDIR}${${prefix}_PLATINCLUDE_PATH}")
  else()
    set(PYTHON_PLATINCLUDE_PATH
        "\$ENV{DESTDIR}${MACHINEKIT_HAL_PYTHON_PLATINCLUDE_FULL_INSTALL_DIRECTORY}"
    )
  endif()
  if(${prefix}_SCRIPTS_PATH)
    set(PYTHON_SCRIPTS_PATH "\$ENV{DESTDIR}${${prefix}_SCRIPTS_PATH}")
  else()
    set(PYTHON_SCRIPTS_PATH
        "\$ENV{DESTDIR}${MACHINEKIT_HAL_PYTHON_SCRIPTS_FULL_INSTALL_DIRECTORY}")
  endif()
  if(${prefix}_DATA_PATH)
    set(PYTHON_DATA_PATH "\$ENV{DESTDIR}${${prefix}_DATA_PATH}")
  else()
    set(PYTHON_DATA_PATH
        "\$ENV{DESTDIR}${MACHINEKIT_HAL_PYTHON_DATA_FULL_INSTALL_DIRECTORY}")
  endif()

  set(EXECUTION_INTERPRETER "${Python_EXECUTABLE}")

  cmake_path(HASH "${prefix}_WHEEL" file_hash)

  set(cmake_script_file
      "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/install_wheel/${file_hash}.cmake")

  file(READ "${installer_cmake_script_template}" cmake_script_template)

  string(CONFIGURE "${cmake_script_template}" cmake_script_configured @ONLY)

  file(
    GENERATE
    OUTPUT "${cmake_script_file}"
    CONTENT "${cmake_script_configured}" NO_SOURCE_PERMISSIONS NEWLINE_STYLE
            UNIX)

  if(${prefix}_COMPONENT)
    install(SCRIPT "${cmake_script_file}" COMPONENT "${${prefix}_COMPONENT}")
  else()
    install(SCRIPT "${cmake_script_file}")
  endif()

endfunction()
