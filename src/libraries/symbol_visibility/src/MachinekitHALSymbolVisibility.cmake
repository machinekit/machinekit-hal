# ~~~
# ####################################################################
# Description:  MachinekitHALSymbolVisibility.cmake
#
#               This file, 'MachinekitHALSymbolVisibility.cmake', is
#               a Machinekit-HAL specific CMake Module used to manipulate
#               a C or C++ target's symbol visibility
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

find_program(BASH "bash" REQUIRED)

set(RTAPI_STRIPPER_SCRIPT
    [=[
#!/bin/env bash

readarray -td\\\; a <<<"$<TARGET_OBJECTS:@PERTINENT_TARGET@>"

GLOBAL_SYMBOLS=""

for i in ${a[@]}
do
    SYMS="$(objcopy -j .rtapi_export -O binary ${i} /dev/stdout | tr -s '\0' | xargs -r0 printf '%s\\;\n' | grep .)"
    GLOBAL_SYMBOLS+="$SYMS"
done

GLOBAL_MAP=""

if [[ "$GLOBAL_SYMBOLS" != "" ]]
then
GLOBAL_MAP=$(printf "%b\n"             \
                    "global :"         \
                    "$GLOBAL_SYMBOLS")
fi

MAP=$(printf "%b\n" \
    "{ $GLOBAL_MAP" \
    "local : * \\;" \
    "}\\;")

echo "$MAP" >@OUTPUT_FILE@
]=])

function(export_rtapi_symbols)
  set(prefix "rtapi_export")
  set(noValues "")
  set(singleValues "TARGET")
  set(multiValues "")
  cmake_parse_arguments(${prefix} "${noValues}" "${singleValues}"
                        "${multiValues}" ${ARGN})

  set(linkerFileDirectory
      "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/linker/${${prefix}_TARGET}")
  set(linkerFileMap "${linkerFileDirectory}/rtapi_export_symbols.map")
  set(linkerFileScript "${linkerFileDirectory}/rtapi_stripper.sh")

  string(REPLACE "@PERTINENT_TARGET@" "${${prefix}_TARGET}"
                 target_stripper_script ${RTAPI_STRIPPER_SCRIPT})
  string(REPLACE "@OUTPUT_FILE@" "${linkerFileMap}" target_stripper_script
                 ${target_stripper_script})

  file(REMOVE_RECURSE "${linkerFileDirectory}")

  # File GENERATE will create the needed directory structure
  file(
    GENERATE
    OUTPUT "${linkerFileScript}"
    CONTENT "${target_stripper_script}"
            TARGET
            ${${prefix}_TARGET}
            NEWLINE_STYLE
            UNIX
            FILE_PERMISSIONS
            OWNER_READ
            OWNER_EXECUTE
            GROUP_READ
            GROUP_EXECUTE
            WORLD_READ
            WORLD_EXECUTE)

  add_custom_command(
    TARGET ${${prefix}_TARGET}
    PRE_LINK
    COMMAND ${CMAKE_COMMAND} "-E" "env" "${BASH}" "${linkerFileScript}"
            BYPRODUCT "${linkerFileMap}"
    VERBATIM)

  target_link_options(${${prefix}_TARGET} PRIVATE
                      "LINKER:--version-script=${linkerFileMap}")

endfunction()
