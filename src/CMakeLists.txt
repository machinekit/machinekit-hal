set(CMAKE_C_FLAGS
    "${CMAKE_C_FLAGS} -fPIC -fwrapv -fno-strict-overflow -std=gnu99 -fgnu89-inline")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

# this copies the neeaded header files to the build directory
include(link_headers)

# copy mank in build directory
add_custom_command(OUTPUT ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/mank
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/bin/mank
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/mank
    DEPENDS ${CMAKE_SOURCE_DIR}/bin/mank)

_install_script(${CMAKE_SOURCE_DIR}/bin/mank)

# make sure the above commands are run
add_custom_target(add_missing ALL
    DEPENDS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/mank ${_headers})

set(CMAKE_LINK_WHAT_YOU_USE ON)

# Use DT_RPATH instead of DT_RUNPATH
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--disable-new-dtags")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--disable-new-dtags")


add_subdirectory(machinetalk)
add_subdirectory(libnml)
add_subdirectory(hal)
add_subdirectory(rtapi)

#[[
add_dependencies(rtapi_app pb_gen_files)
foreach(_flav ${BUILD_THREAD_FLAVORS})
    _flavor_helper(${_flav})
    add_dependencies(pbring.${_fname} npb_gen_files)
    add_dependencies(pbmsgs.${_fname} npb_gen_files)
    add_dependencies(ringload.${_fname} npb_gen_files)
    add_dependencies(hal_lib_${_fname} pb_gen_files)
endforeach()
]]

# create needed directories
file(MAKE_DIRECTORY ${PROJECT_MAN_DIR}/man1)
file(MAKE_DIRECTORY ${PROJECT_MAN_DIR}/man3)
file(MAKE_DIRECTORY ${PROJECT_MAN_DIR}/man9)
file(MAKE_DIRECTORY ${PROJECT_SYSCONF_DIR})
