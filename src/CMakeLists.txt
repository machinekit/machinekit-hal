set(CMAKE_LINK_WHAT_YOU_USE ON)

# Use DT_RPATH instead of DT_RUNPATH
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--disable-new-dtags")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--disable-new-dtags")

set(CMAKE_C_FLAGS
    "${CMAKE_C_FLAGS} -fPIC -fwrapv -fno-strict-overflow -std=gnu99 -fgnu89-inline")

# jessie quirk
if(RELEASE_NUMBER VERSION_LESS "9")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

# this copies the neeaded header files to the build directory
include(link_headers)

# copy mank in build directory
add_custom_command(OUTPUT ${PROJECT_BIN_DIR}/mank
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/bin/mank
        ${PROJECT_BIN_DIR}/mank
    DEPENDS ${CMAKE_SOURCE_DIR}/bin/mank)

_install_script(${CMAKE_SOURCE_DIR}/bin/mank)

# make sure the above commands are run
add_custom_target(copy_mank ALL DEPENDS ${PROJECT_BIN_DIR}/mank)

add_subdirectory(machinetalk)
add_subdirectory(libnml)
add_subdirectory(hal)
add_subdirectory(rtapi)

# process python files
set(_mk_dep ${CMAKE_SOURCE_DIR}/lib/python/machinekit/halfile.py
    ${CMAKE_SOURCE_DIR}/lib/python/machinekit/launcher.py
    ${CMAKE_SOURCE_DIR}/lib/python/machinekit/service.py
    ${CMAKE_SOURCE_DIR}/lib/python/machinekit/nosetests)

set(_py_dep ${CMAKE_SOURCE_DIR}/lib/python/drivers
    ${CMAKE_SOURCE_DIR}/lib/python/gladevcp
    ${CMAKE_SOURCE_DIR}/lib/python/bitfile.py
    ${CMAKE_SOURCE_DIR}/lib/python/bwidget.py
    ${CMAKE_SOURCE_DIR}/lib/python/gladevcp_makepins.py
    ${CMAKE_SOURCE_DIR}/lib/python/hal_glib.py
    ${CMAKE_SOURCE_DIR}/lib/python/linux_event.py
    ${CMAKE_SOURCE_DIR}/lib/python/pyvcp_widgets.py
    ${CMAKE_SOURCE_DIR}/lib/python/servicediscovery.py)

add_custom_target(mk_dep ALL
    COMMAND ${CMAKE_COMMAND}
        -E make_directory ${PROJECT_PYTHON_DIR}/machinekit/protobuf
    COMMAND ${CMAKE_COMMAND}
        -E touch ${PROJECT_PYTHON_DIR}/machinekit/__init__.py
    COMMAND ${CMAKE_COMMAND}
        -E touch ${PROJECT_PYTHON_DIR}/machinekit/protobuf/__init__.py
    COMMAND cp -af ${_mk_dep} ${PROJECT_PYTHON_DIR}/machinekit
    DEPENDS ${_mk_dep})

add_custom_target(py_deps ALL
    COMMAND cp -af ${_py_dep} ${PROJECT_PYTHON_DIR}
    DEPENDS ${_py_dep})

# create setup.py
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/setup.py.in"
    "${PROJECT_PYTHON_DIR}/setup.py")

# install python files
install(CODE "execute_process(
                            COMMAND ${CMAKE_MODULE_PATH}/python_package_helper
                                    ${Python2_EXECUTABLE}
                                    ${PROJECT_PYTHON_DIR})")

# create needed directories
file(MAKE_DIRECTORY ${PROJECT_MAN_DIR}/man1)
file(MAKE_DIRECTORY ${PROJECT_MAN_DIR}/man3)
file(MAKE_DIRECTORY ${PROJECT_MAN_DIR}/man9)
file(MAKE_DIRECTORY ${PROJECT_SYSCONF_DIR})

set(_src ${CMAKE_BINARY_DIR}/include)
set(_dst ${CMAKE_INSTALL_PREFIX}/include/machinekit)
# install headers, convert symlinks to regular files
install(CODE "execute_process(COMMAND \
        sh -c \" mkdir -p $DESTDIR${_dst}; \
                 tar -chf - -C ${_src} . | tar -xf - -C $DESTDIR${_dst} \
              \")\
            ")
