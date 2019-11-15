add_subdirectory(shmdrv)

foreach(file ulapi_autoload rtapi_compat rtapi_support rtapi_heap rtapi_hexdump)
    add_library(${file} OBJECT ${file}.c)
    target_compile_definitions(${file} PRIVATE "ULAPI")
endforeach()

target_compile_definitions(rtapi_compat PRIVATE
    PB_FIELD_32BIT "EMC2_SYSTEM_CONFIG_DIR=\"${EMC2_SYSTEM_CONFIG_DIR}\"")

add_subdirectory(rtapi_math)

add_library(mkulapi SHARED $<TARGET_OBJECTS:ulapi_autoload>
    $<TARGET_OBJECTS:rtapi_compat> $<TARGET_OBJECTS:rtapi_support>
    $<TARGET_OBJECTS:rtapi_hexdump> $<TARGET_OBJECTS:syslog_async>)
target_link_libraries(mkulapi mkini mkshm ${CMAKE_DL_LIBS})
set_target_properties(mkulapi PROPERTIES
    COMPILE_DEFINITIONS "ULAPI"
    BUILD_WITH_INSTALL_RPATH FALSE
    INSTALL_RPATH "${_rpath}"
    INSTALL_RPATH_USE_LINK_PATH TRUE
    VERSION ${GENERIC_LIB_VERSION})
rpath_hack(mkulapi)
_install(mkulapi)

add_executable(rtapi_kdetect rtapi_kdetect.c)
set_target_properties(rtapi_kdetect PROPERTIES
        COMPILE_DEFINITIONS "ULAPI;TEST"
        RUNTIME_OUTPUT_DIRECTORY ${PROJECT_LIBEXEC_DIR})
target_link_libraries(rtapi_kdetect mkhal)
_install_exec(rtapi_kdetect)

add_executable(rtapi_msgd rtapi_msgd.cc $<TARGET_OBJECTS:rtapi_support>
    $<TARGET_OBJECTS:rtapi_compat> $<TARGET_OBJECTS:rtapi_heap>)
set_target_properties(rtapi_msgd PROPERTIES
    COMPILE_DEFINITIONS "ULAPI;GIT_BUILD_SHA=\"${_out}\""
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_LIBEXEC_DIR})
target_link_libraries(rtapi_msgd mtalk mktalk-pb2++ mkini mkshm)
_install_exec(rtapi_msgd)

add_executable(flavor flavor.c $<TARGET_OBJECTS:rtapi_compat>)
set_target_properties(flavor PROPERTIES
    COMPILE_DEFINITIONS "ULAPI"
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_LIBEXEC_DIR})
target_link_libraries(flavor mkini mkshm)
_install_exec(flavor)

add_definitions(-DEMC2_SYSTEM_CONFIG_DIR=\"${EMC2_SYSTEM_CONFIG_DIR}\")

set(XXAPI_COMMON_SRCS
    rtapi_common.c
    rtapi_task.c
    rtapi_shmem.c
    rtapi_time.c
    rtapi_exception.c)

add_library(rtapi_app OBJECT rtapi_app.cc)
set_target_properties(rtapi_app PROPERTIES COMPILE_DEFINITIONS "ULAPI")
add_dependencies(rtapi_app npb_gen_files)

set(RTAPI_SRC ${XXAPI_COMMON_SRCS}
    rtapi_heap.c rtapi_main.c rtapi_support.c rtapi_hexdump.c rtapi_compat.c
    $<TARGET_OBJECTS:nanopb>)

unset(cflags)
if(WITH_USERMODE_PCI)
    set(RTAPI_SRC ${RTAPI_SRC} rtapi_pci.c)
    if(UDEV_CFLAGS)
        set(cflags "${cflags} ${UDEV_CFLAGS}")
    endif()
endif()

foreach(_flav ${BUILD_THREAD_FLAVORS})
    _flavor_helper(${_flav})
    add_library(ulapi-${_fname} SHARED ${XXAPI_COMMON_SRCS}
            ulapi_main.c ${${_FLAV}_THREADS_SOURCE}
            $<TARGET_OBJECTS:rtapi_heap> $<TARGET_OBJECTS:rtapi_support>)
    set_target_properties(ulapi-${_fname} PROPERTIES
            COMPILE_DEFINITIONS "ULAPI;THREAD_FLAVOR_ID=${RTAPI_${_FLAV}_ID}"
            COMPILE_FLAGS "${${_FLAV}_THREADS_RTFLAGS}"
            LIBRARY_OUTPUT_DIRECTORY ${PROJECT_LIBEXEC_DIR}
            LINK_FLAGS "${${_FLAV}_THREADS_LDFLAGS}"
            PREFIX "")
    target_link_libraries(ulapi-${_fname} mkshm mkhal)
    _install_exec(ulapi-${_fname})

    add_library(rtapi_${_fname} SHARED ${RTAPI_SRC} ${${_FLAV}_THREADS_SOURCE})
    set_target_properties(rtapi_${_fname} PROPERTIES
            COMPILE_DEFINITIONS "RTAPI;THREAD_FLAVOR_ID=${RTAPI_${_FLAV}_ID}"
            COMPILE_FLAGS "${${_FLAV}_THREADS_RTFLAGS} ${cflags}"
            LIBRARY_OUTPUT_DIRECTORY ${PROJECT_LIBEXEC_DIR}/${_fname}
            LINK_FLAGS "${${_FLAV}_THREADS_LDFLAGS} -Wl,--no-as-needed"
            OUTPUT_NAME "rtapi"
            PREFIX "")
    set(RTAPI_LIBS ${LCG_LIBRARIES})
    if(WITH_USERMODE_PCI)
            set(RTAPI_LIBS ${RTAPI_LIBS} ${UDEV_LIBRARIES})
    endif()
    target_link_libraries(rtapi_${_fname} ${RTAPI_LIBS})
    _install_exec(rtapi_${_fname} ${_fname})

    add_executable(rtapi_app_${_fname} rtapi_app.cc rtapi_compat.c
            rtapi_hexdump.c rtapi_support.c)
    set_target_properties(rtapi_app_${_fname} PROPERTIES
            COMPILE_DEFINITIONS "ULAPI;THREAD_FLAVOR_ID=${RTAPI_${_FLAV}_ID}"
            COMPILE_FLAGS "${${_FLAV}_THREADS_RTFLAGS}"
            BUILD_WITH_INSTALL_RPATH FALSE
            INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib/machinekit/${_fname};${_rpath}"
            INSTALL_RPATH_USE_LINK_PATH TRUE
            LINK_FLAGS "${${_FLAV}_THREADS_LDFLAGS}"
            RUNTIME_OUTPUT_DIRECTORY ${PROJECT_LIBEXEC_DIR})
    target_link_libraries(rtapi_app_${_fname} mkini
        mkshm mtalk rtapi_math mktalk-pb2++ ${CMAKE_DL_LIBS})
    rpath_hack_flavor(rtapi_app_${_fname})
    _install_exec_setuid(rtapi_app_${_fname})

    set(SETUID_DEPS ${SETUID_DEPS} rtapi_app_${_fname})
endforeach()

add_custom_target(setuid
    COMMAND chown 0:0 ${SETUID_DEPS}
    COMMAND chmod 4755 ${SETUID_DEPS}
    DEPENDS ${SETUID_DEPS}
    WORKING_DIRECTORY ${PROJECT_LIBEXEC_DIR}
    VERBATIM)

install(FILES rsyslogd-machinekit.conf
    DESTINATION /etc/rsyslog.d
    RENAME machinekit.conf)
