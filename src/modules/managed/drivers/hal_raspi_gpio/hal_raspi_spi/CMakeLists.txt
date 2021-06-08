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

if(CMAKE_SYSTEM_PROCESSOR STREQUAL "armhf")
  option(BUILD_HAL_RASPI_SPI_MANAGED_DRIVER_MODULE
         "Built the Raspberry Pi SPI HAL managed driver module." TRUE)

  if(BUILD_HAL_RASPI_SPI_MANAGED_DRIVER_MODULE)
    include(MachinekitHALSymbolVisibility)

    add_library(hal_raspi_spi MODULE)

    set(SOURCE_FILES src/cpuinfo.c src/hal_spi.c)

    set(PRIVATE_HEADER_FILES src/cpuinfo.h src/hal_spi.h)

    target_sources(hal_raspi_spi PRIVATE ${SOURCE_FILES}
                                         ${PRIVATE_HEADER_FILES})

    target_link_libraries(hal_raspi_spi PRIVATE hal_api runtime_api
                                                runtime_math raspi_cpuinfo)

    export_rtapi_symbols(TARGET hal_raspi_spi)

    target_compile_definitions(hal_raspi_spi PRIVATE "RTAPI")

    set_target_properties(
      hal_raspi_spi
      PROPERTIES PREFIX "mod"
                 LIBRARY_OUTPUT_DIRECTORY
                 "${MACHINEKIT_HAL_MANAGED_MODULE_OUTPUT_DIRECTORY}")

    install(TARGETS hal_raspi_spi
            LIBRARY DESTINATION "${MACHINEKIT_HAL_MANAGED_MODULE_DIRECTORY}"
                    COMPONENT MachinekitHAL_Managed_Driver_Module_HAL_RASPI_SPI)
  endif()
endif()
