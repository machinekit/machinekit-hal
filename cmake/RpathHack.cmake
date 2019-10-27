# this is a hack to allow RUN_IN_PLACE in the build directory
# cmake currently does not allow directly setting the RPATH value

set(_mkrip ${CMAKE_CURRENT_BINARY_DIR}/mk_rip.c)
add_custom_command(
    OUTPUT ${_mkrip}
    COMMAND ${CMAKE_COMMAND} -E touch ${_mkrip})

add_library(mk_rip SHARED ${_mkrip})
set_target_properties(mk_rip PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${PROJECT_LIBEXEC_DIR}
    OUTPUT_NAME mk_rip
    PREFIX ""
    SUFFIX "")

foreach(_flav ${BUILD_THREAD_FLAVORS})
    _flavor_helper(${_flav})
    add_library(mk_rip.${_fname} SHARED ${CMAKE_CURRENT_BINARY_DIR}/mk_rip.c)
    set_target_properties(mk_rip.${_fname} PROPERTIES
            LIBRARY_OUTPUT_DIRECTORY ${PROJECT_LIBEXEC_DIR}/${_fname}
        OUTPUT_NAME mk_rip
            PREFIX ""
            SUFFIX "")
endforeach()

# set the RPATH for installed binaries, but only if it's not a system directory
set(_rpath "${CMAKE_INSTALL_PREFIX}/lib/machinekit")
list(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES
    "${CMAKE_INSTALL_PREFIX}/lib" _res)
if("${_res}" STREQUAL "-1")
    set(_rpath  ${_rpath} "${CMAKE_INSTALL_PREFIX}/lib")
endif()

#! rpath_hack : add RPATH to built binaries to enable RIP mode
#
# linking to mk_rip forces cmake to add the directory to the RPATH during build step
# and the dummy library is then removed afterwards
#
# \arg:TGT cmake executable/library target
macro(rpath_hack TGT)
    target_link_libraries(${TGT} mk_rip)
    add_custom_command(TARGET ${TGT}
            POST_BUILD
        COMMAND patchelf --remove-needed mk_rip $<TARGET_FILE:${TGT}>)
endmacro()

macro(rpath_hack_flavor TGT)
    target_link_libraries(${TGT} mk_rip.${_fname})
    add_custom_command(TARGET ${TGT}
            POST_BUILD
        COMMAND patchelf --remove-needed mk_rip $<TARGET_FILE:${TGT}>)
endmacro()
