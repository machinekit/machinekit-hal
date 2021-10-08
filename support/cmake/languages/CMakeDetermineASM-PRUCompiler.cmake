# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

# determine the compiler to use for ASM using TI PRU syntax

find_program(PASM "pasm" REQUIRED)

set(ASM_DIALECT "-PRU")
set(CMAKE_ASM${ASM_DIALECT}_COMPILER_LIST ${PASM})
include(CMakeDetermineASMCompiler)
set(ASM_DIALECT)
