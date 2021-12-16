# ~~~
# ####################################################################
# Description:  MachinekitHALCompFunction.cmake
#
#               This file, 'MachinekitHALCompFunction.cmake', implements
#               CMake module to build Machinekit-HAL managed HAL modules
#               from the '.comp' template files
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

# TODO: Separate common functionality into third function

if(NOT DEFINED MACHINEKIT_HAL_COMP)
  get_property(
    _comp_set GLOBAL
    PROPERTY MACHINEKIT_HAL_COMP
    SET)
  if(_comp_set)
    get_property(MACHINEKIT_HAL_COMP GLOBAL PROPERTY MACHINEKIT_HAL_COMP)
  else()
    find_program(MACHINEKIT_HAL_COMP "comp" REQUIRED)
  endif()
  unset(_comp_set)
endif()

if(NOT DEFINED export_rtapi_symbols)
  include(MachinekitHALSymbolVisibilityFunction)
endif()

function(add_legacy_managed_module target_name)
  set(prefix "comp")
  set(noValues "")
  set(singleValues "OUTPUT_DIRECTORY" "SOURCE" "INSTALL_DIRECTORY"
                   "CPACK_OUTPUT_GROUP")
  set(multiValues "LINK_LIBRARIES" "CPACK_DEPENDENCY_COMPONENTS")

  cmake_parse_arguments(PARSE_ARGV 1 "${prefix}" "${noValues}"
                        "${singleValues}" "${multiValues}")

  if(${prefix}_KEYWORDS_MISSING_VALUES)
    message(
      FATAL_ERROR
        "All keyword arguments need values! (${${prefix}_KEYWORDS_MISSING_VALUES})"
    )
  endif()

  list(APPEND all_key_arguments "${noValues}" "${singleValues}"
       "${multiValues}")
  if(NOT target_name OR ${target_name} IN_LIST all_key_arguments)
    message(FATAL_ERROR "Target name has to be specified!")
  endif()

  if(NOT ${prefix}_SOURCE)
    message(
      FATAL_ERROR
        "Target source file has to be specified via the SOURCE named argument!")
  endif()

  cmake_path(ABSOLUTE_PATH ${prefix}_SOURCE NORMALIZE OUTPUT_VARIABLE
             source_file)
  cmake_path(GET source_file EXTENSION source_file_extension)
  cmake_path(GET source_file FILENAME source_file_filename)
  cmake_path(GET source_file STEM source_file_stem)

  if(NOT source_file_extension STREQUAL ".comp")
    message(
      FATAL_ERROR "File passed as SOURCE has to have the '.comp' extension!")
  endif()

  set(output_build_directory
      "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/src/${target_name}")
  set(preprocessed_c_source_file
      "${output_build_directory}/${source_file_stem}.c")

  add_custom_command(
    OUTPUT "${preprocessed_c_source_file}"
    COMMAND ${CMAKE_COMMAND} "-E" "make_directory" "${output_build_directory}"
    COMMAND ${CMAKE_COMMAND} "-E" "env" "${MACHINEKIT_HAL_COMP}" "--preprocess"
            "--outfile" "${preprocessed_c_source_file}" "${source_file}"
    MAIN_DEPENDENCY "${source_file}"
    COMMENT "Preprocessing legacy module ${source_file_filename} to C file")

  add_library(${target_name} MODULE)
  target_sources(${target_name} PRIVATE ${preprocessed_c_source_file})

  target_link_libraries(${target_name} PRIVATE managed_hal managed_runtime)

  if(${prefix}_LINK_LIBRARIES)
    target_link_libraries(${target_name} PRIVATE ${${prefix}_LINK_LIBRARIES})
  endif()

  export_rtapi_symbols(TARGET ${target_name})

  set_target_properties(
    ${target_name} PROPERTIES OUTPUT_NAME "${source_file_stem}" PREFIX "mod")

  if(${prefix}_OUTPUT_DIRECTORY)
    cmake_path(IS_ABSOLUTE ${prefix}_OUTPUT_DIRECTORY
               output_directory_is_absolute)
    if(NOT output_directory_is_absolute)
      message(FATAL_ERROR "Path passed as OUTPUT_DIRECTORY must be absolute!")
    endif()

    set_target_properties(
      ${target_name} PROPERTIES LIBRARY_OUTPUT_DIRECTORY
                                "${${prefix}_OUTPUT_DIRECTORY}")
  endif()

  if(${prefix}_INSTALL_DIRECTORY)
    install(
      TARGETS ${target_name}
      LIBRARY
        DESTINATION "${${prefix}_INSTALL_DIRECTORY}"
        COMPONENT MachinekitHAL_Managed_Module_${target_name}_Legacy_Components)
  endif()

  if(${prefix}_CPACK_OUTPUT_GROUP)
    if(${prefix}_CPACK_DEPENDENCY_COMPONENTS)
      cpack_add_component(
        MachinekitHAL_Managed_Module_${target_name}_Legacy_Components
        GROUP MachinekitHAL_Managed_Module_${target_name}
        DEPENDS "${${prefix}_CPACK_DEPENDENCY_COMPONENTS}")
    else()
      cpack_add_component(
        MachinekitHAL_Managed_Module_${target_name}_Legacy_Components
        GROUP MachinekitHAL_Managed_Module_${target_name})
    endif()

    # Specification of artifacts placement in package tree
    cpack_add_component_group(MachinekitHAL_Managed_Module_${target_name}
                              PARENT_GROUP ${${prefix}_CPACK_OUTPUT_GROUP})
  endif()
endfunction()

function(add_legacy_unmanaged_module target_name)
  set(prefix "comp")
  set(noValues "")
  set(singleValues "OUTPUT_DIRECTORY" "SOURCE" "INSTALL_DIRECTORY"
                   "CPACK_OUTPUT_GROUP")
  set(multiValues "LINK_LIBRARIES" "CPACK_DEPENDENCY_COMPONENTS")

  cmake_parse_arguments(PARSE_ARGV 1 "${prefix}" "${noValues}"
                        "${singleValues}" "${multiValues}")

  if(${prefix}_KEYWORDS_MISSING_VALUES)
    message(
      FATAL_ERROR
        "All keyword arguments need values! (${${prefix}_KEYWORDS_MISSING_VALUES})"
    )
  endif()

  list(APPEND all_key_arguments "${noValues}" "${singleValues}"
       "${multiValues}")
  if(NOT target_name OR ${target_name} IN_LIST all_key_arguments)
    message(FATAL_ERROR "Target name has to be specified!")
  endif()

  if(NOT ${prefix}_SOURCE)
    message(
      FATAL_ERROR
        "Target source file has to be specified via the SOURCE named argument!")
  endif()

  cmake_path(ABSOLUTE_PATH ${prefix}_SOURCE NORMALIZE OUTPUT_VARIABLE
             source_file)
  cmake_path(GET source_file EXTENSION source_file_extension)
  cmake_path(GET source_file FILENAME source_file_filename)
  cmake_path(GET source_file STEM source_file_stem)

  if(NOT source_file_extension STREQUAL ".comp")
    message(
      FATAL_ERROR "File passed as SOURCE has to have the '.comp' extension!")
  endif()

  set(output_build_directory
      "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/src/${target_name}")
  set(preprocessed_c_source_file
      "${output_build_directory}/${source_file_stem}.c")

  add_custom_command(
    OUTPUT "${preprocessed_c_source_file}"
    COMMAND ${CMAKE_COMMAND} "-E" "make_directory" "${output_build_directory}"
    COMMAND ${CMAKE_COMMAND} "-E" "env" "${MACHINEKIT_HAL_COMP}" "--preprocess"
            "--outfile" "${preprocessed_c_source_file}" "${source_file}"
    MAIN_DEPENDENCY "${source_file}"
    COMMENT "Preprocessing legacy module ${source_file_filename} to C file")

  add_executable(${target_name})
  target_sources(${target_name} PRIVATE ${preprocessed_c_source_file})

  target_link_libraries(${target_name} PRIVATE unmanaged_hal unmanaged_runtime)

  if(${prefix}_LINK_LIBRARIES)
    target_link_libraries(${target_name} PRIVATE ${${prefix}_LINK_LIBRARIES})
  endif()

  if(${prefix}_OUTPUT_DIRECTORY)
    cmake_path(IS_ABSOLUTE ${prefix}_OUTPUT_DIRECTORY
               output_directory_is_absolute)
    if(NOT output_directory_is_absolute)
      message(FATAL_ERROR "Path passed as OUTPUT_DIRECTORY must be absolute!")
    endif()

    set_target_properties(
      ${target_name} PROPERTIES LIBRARY_OUTPUT_DIRECTORY
                                "${${prefix}_OUTPUT_DIRECTORY}")
  endif()

  if(${prefix}_INSTALL_DIRECTORY)
    install(
      TARGETS ${target_name}
      RUNTIME
        DESTINATION "${${prefix}_INSTALL_DIRECTORY}"
        COMPONENT
          MachinekitHAL_Unmanaged_Module_${target_name}_Legacy_Components)
  endif()

  if(${prefix}_CPACK_OUTPUT_GROUP)
    if(${prefix}_CPACK_DEPENDENCY_COMPONENTS)
      cpack_add_component(
        MachinekitHAL_Unmanaged_Module_${target_name}_Legacy_Components
        GROUP MachinekitHAL_Unmanaged_Module_${target_name}
        DEPENDS "${${prefix}_CPACK_DEPENDENCY_COMPONENTS}")
    else()
      cpack_add_component(
        MachinekitHAL_Unmanaged_Module_${target_name}_Legacy_Components
        GROUP MachinekitHAL_Unmanaged_Module_${target_name})
    endif()

    # Specification of artifacts placement in package tree
    cpack_add_component_group(MachinekitHAL_Unmanaged_Module_${target_name}
                              PARENT_GROUP ${${prefix}_CPACK_OUTPUT_GROUP})
  endif()
endfunction()
