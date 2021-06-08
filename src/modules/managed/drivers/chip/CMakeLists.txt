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
  option(BUILD_HAL_CHIP_GPIO_MANAGED_DRIVER_MODULE
         "Built the Chip HAL managed driver module." TRUE)

  if(BUILD_HAL_CHIP_GPIO_MANAGED_DRIVER_MODULE)
    include(MachinekitHALSymbolVisibility)

    add_library(hal_chip_gpio MODULE)

    set(SOURCE_FILES src/hal_chip_gpio.c src/mmap_gpio.c)

    set(PRIVATE_HEADER_FILES src/libsoc_mmap_gpio.h)

    target_sources(hal_chip_gpio PRIVATE ${SOURCE_FILES}
                                         ${PRIVATE_HEADER_FILES})

    target_link_libraries(hal_chip_gpio PRIVATE hal_api runtime_api)

    export_rtapi_symbols(TARGET hal_chip_gpio)

    target_compile_definitions(hal_chip_gpio PRIVATE "RTAPI")

    set_target_properties(
      hal_chip_gpio
      PROPERTIES PREFIX "mod"
                 LIBRARY_OUTPUT_DIRECTORY
                 "${MACHINEKIT_HAL_MANAGED_MODULE_OUTPUT_DIRECTORY}")

    install(
      TARGETS hal_chip_gpio
      EXPORT machinekit_hal_managed_driver_module_hal_chip_gpio
      LIBRARY DESTINATION "${MACHINEKIT_HAL_MANAGED_MODULE_DIRECTORY}"
              COMPONENT MachinekitHAL_Managed_Driver_Module_HAL_Chip_GPIO)
  endif()
endif()
