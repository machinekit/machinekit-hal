
set(HALLIBSRCS
    hal_lib.c
    hal_group.c
    hal_ring.c
    hal_rcomp.c
    hal_vtable.c
    hal_funct.c
    hal_procfs.c
    hal_thread.c
    hal_param.c
    hal_signal.c
    hal_pin.c
    hal_comp.c
    hal_memory.c
    hal_misc.c
    hal_instance.c
    hal_object.c
    hal_object_selectors.c
    hal_accessor.c
    hal_iring.c)

add_library(linuxcnchal SHARED ${HALLIBSRCS} halpb.cc
    $<TARGET_OBJECTS:inifile> $<TARGET_OBJECTS:shmdrvapi>
    $<TARGET_OBJECTS:ulapi_autoload> $<TARGET_OBJECTS:rtapi_compat>
    $<TARGET_OBJECTS:rtapi_support> $<TARGET_OBJECTS:rtapi_heap>
    $<TARGET_OBJECTS:rtapi_hexdump> $<TARGET_OBJECTS:syslog_async>)
target_link_libraries(linuxcnchal machinetalk-pb2++ ${CMAKE_DL_LIBS} rt)
set_target_properties(linuxcnchal PROPERTIES
    COMPILE_DEFINITIONS "ULAPI;THREAD_FLAVOR_ID=0"
    VERSION ${GENERIC_LIB_VERSION})
_install(linuxcnchal)

set(halpy_dir ${PROJECT_PYTHON_DIR}/hal)
set(halpy ${halpy_dir}/__init__.py)
add_custom_command(OUTPUT ${halpy}
    COMMAND ${CMAKE_COMMAND} -E copy hal.py ${halpy}
    DEPENDS hal.py
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
add_custom_target(halpy DEPENDS ${halpy})

add_library(_hal SHARED halmodule.cc)
set_property(TARGET _hal APPEND PROPERTY
    INCLUDE_DIRECTORIES ${PYTHON_INCLUDE_DIRS})
set_target_properties(_hal PROPERTIES
    BUILD_WITH_INSTALL_RPATH FALSE
    INSTALL_RPATH "${_rpath}"
    INSTALL_RPATH_USE_LINK_PATH TRUE
    COMPILE_DEFINITIONS "ULAPI"
    LIBRARY_OUTPUT_DIRECTORY ${halpy_dir}
    PREFIX "")
target_link_libraries(_hal  linuxcnchal ${PYTHON_LIBRARIES})
add_dependencies(_hal halpy)
cleanup_rpath_hack(_hal)
_install_py(hal _hal)

foreach(_flav ${BUILD_THREAD_FLAVORS})
    _flavor_helper(${_flav})
    add_library(hal_lib_${_fname} SHARED ${HALLIBSRCS})
    set_target_properties(hal_lib_${_fname} PROPERTIES
        COMPILE_DEFINITIONS "RTAPI;THREAD_FLAVOR_ID=${RTAPI_${_FLAV}_ID}"
        COMPILE_FLAGS "${${_FLAV}_THREADS_RTFLAGS}"
        LIBRARY_OUTPUT_DIRECTORY ${PROJECT_LIBEXEC_DIR}/${_fname}
        OUTPUT_NAME "hal_lib"
        PREFIX "")
    _install_exec(hal_lib_${_fname} ${_fname})
endforeach()
