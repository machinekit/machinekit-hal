# this is a hack to allow RUN_IN_PLACE on the build directory
set(_mkrip ${CMAKE_CURRENT_BINARY_DIR}/mk_rip.c)
add_custom_command(
    OUTPUT ${_mkrip}
    COMMAND ${CMAKE_COMMAND} -E touch ${_mkrip})

add_library(mk_rip SHARED ${_mkrip})
set_target_properties(mk_rip PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${PROJECT_LIBEXEC_DIR}
    OUTPUT_NAME .rIp.0.Me.
    PREFIX ""
    SUFFIX "")
foreach(_flav ${BUILD_THREAD_FLAVORS})
    _flavor_helper(${_flav})
    add_library(mk_rip.${_fname} SHARED ${CMAKE_CURRENT_BINARY_DIR}/mk_rip.c)
    set_target_properties(mk_rip.${_fname} PROPERTIES
            LIBRARY_OUTPUT_DIRECTORY ${PROJECT_LIBEXEC_DIR}/${_fname}
            OUTPUT_NAME .rIp.1.Me.
            PREFIX ""
            SUFFIX "")
    target_link_libraries(mk_rip.${_fname})
endforeach()

# set the RPATH for installed binaries, but only if it's not a system directory
set(_rpath "${CMAKE_INSTALL_PREFIX}/lib/machinekit")
list(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES
    "${CMAKE_INSTALL_PREFIX}/lib" _res)
if("${_res}" STREQUAL "-1")
    set(_rpath  ${_rpath} "${CMAKE_INSTALL_PREFIX}/lib")
endif()

#! cleanup_rpath_hack : remove the linked mk_rip target
#
# This macro deletes the mk_rip target from the dynamic section of the ELF header.
#
# \arg:TGT cmake executable/library target
macro(cleanup_rpath_hack TGT)
    add_custom_command(TARGET ${TGT}
            POST_BUILD
            COMMAND sed -i
            \"s-\\.rIp\\..\\.Me\\.-\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00-g\"
            $<TARGET_FILE:${TGT}>
            COMMENT "removing mk_rip from ${TGT}"
            )
endmacro()
