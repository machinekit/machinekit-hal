# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

# support for TI PRU assembler

# TODO: This whole code and workflow is iffy and quite hackish, would be best if
# somebody cleaned it up

set(ASM_DIALECT "-PRU")
# *.S files are supposed to be preprocessed, so they should not be passed to
# assembler but should be processed by gcc
set(CMAKE_ASM${ASM_DIALECT}_SOURCE_FILE_EXTENSIONS p)

# The version of TI PRU Assembler used for Machinekit-HAL's managed driver
# HAL_PRU_Generic fails on any '.' (dot) in the path, so we cannot use the
# <OBJECT_DIR> in any form ('CMakeFiles/pru_generic.dir')
#
# This also means that whoever will use this (downstream) will need to make sure
# he is following the requirement
#
# Because the Ninja build-system generator for some reason does not recognize
# the <TARGET> build rule substitution in COMPILE_OBJECT step (and Make has a problem
# with copying the built artifacts to the target output directory), use this hack
# to make both Make and Ninja working and happy
if(CMAKE_GENERATOR MATCHES "Ninja")
  set(CMAKE_ASM${ASM_DIALECT}_COMPILE_OBJECT
      "mkdir -p $$(echo \"<OBJECT>\" | sed 's/\\.//g')"
      "<CMAKE_ASM${ASM_DIALECT}_COMPILER> <FLAGS> -b -d <SOURCE> $$(echo \"<OBJECT>\" | sed 's/\\.//g')"
  )
elseif(CMAKE_GENERATOR MATCHES "Make")
  set(CMAKE_ASM${ASM_DIALECT}_COMPILE_OBJECT
      "<CMAKE_ASM${ASM_DIALECT}_COMPILER> <FLAGS> -b -d <SOURCE> <TARGET>")
endif()

include(CMakeASMInformation)

# Remove linking step/options added by the CMakeASMInformation as the PRU
# Assembler doesn't need additional linking step, at least not for the current
# Machinekit-HAL implementation
if(CMAKE_GENERATOR MATCHES "Make")
  set(CMAKE_ASM${ASM_DIALECT}_LINK_EXECUTABLE "")
  set(CMAKE_ASM${ASM_DIALECT}_OUTPUT_EXTENSION ".bin")
elseif(CMAKE_GENERATOR MATCHES "Ninja")
  set(CMAKE_ASM${ASM_DIALECT}_LINK_EXECUTABLE
      "${CMAKE_COMMAND} -E copy_if_different $$(echo \"<OBJECTS>\" | sed 's/\\.//g').bin <TARGET>.bin"
      "${CMAKE_COMMAND} -E copy_if_different $$(echo \"<OBJECTS>\" | sed 's/\\.//g').dbg <TARGET>.dbg"
  )
  set(CMAKE_ASM${ASM_DIALECT}_OUTPUT_EXTENSION "")
endif()

set(CMAKE_ASM${ASM_DIALECT}_LINKER_LAUNCHER "")

set(CMAKE_ASM${ASM_DIALECT}_OUTPUT_EXTENSION_REPLACE 1)

set(ASM_DIALECT)
