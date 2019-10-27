# TODO: replace all EMC2xx variables
set(REALTIME ${PROJECT_BIN_DIR}/realtime)
set(EMC2_BIN_DIR ${PROJECT_BIN_DIR})
set(EMC2_RTLIB_DIR ${PROJECT_LIBEXEC_DIR})
set(prefix ${PROJECT_BINARY_DIR})
set(exec_prefix ${PROJECT_BINARY_DIR})
set(EMC2_SYSTEM_CONFIG_DIR ${PROJECT_SYSCONF_DIR}/machinekit)
set(EMC2_LIBEXEC_DIR ${PROJECT_LIBEXEC_DIR})
set(EMC2_TCL_LIB_DIR ${PROJECT_TCL_DIR})

set(TCLSH ${TCL_TCLSH})

set(RUN_IN_PLACE "yes")
string(TIMESTAMP DATE UTC)
set(configure_input "built with cmake")

configure_file("rtapi.in" "${PROJECT_SYSCONF_DIR}/machinekit/rtapi.ini" @ONLY)
configure_file("set_env.in" "${PROJECT_BIN_DIR}/set_env" @ONLY)
configure_file("halrun.in" "halrun" @ONLY)
configure_file("realtime.in" "realtime" @ONLY)
configure_file("machinekit.ini.tmpl.in" "${PROJECT_SYSCONF_DIR}/machinekit/machinekit.ini" @ONLY)
configure_file("runtests.in" "runtests" @ONLY)
configure_file("haltcl.in" "haltcl" @ONLY)

string(REPLACE ";" " " _BUILD_THREAD_FLAVORS "${BUILD_THREAD_FLAVORS}")
configure_file("${CMAKE_SOURCE_DIR}/src/Makefile.in" "${PROJECT_DATA_DIR}/Makefile.modinc" @ONLY)

# create exec
foreach(tool realtime halrun runtests haltcl)
    file(COPY ${CMAKE_CURRENT_BINARY_DIR}/${tool}
        DESTINATION ${PROJECT_BIN_DIR}
        FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ
        GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
endforeach()

# create files for installation
set(PROJECT_SYSCONF_DIR "${INSTALL_SYSCONF_DIR}")
set(PROJECT_LIBEXEC_DIR "${CMAKE_INSTALL_PREFIX}/lib/machinekit")
set(PROJECT_BINARY_DIR "${CMAKE_INSTALL_PREFIX}")
set(PROJECT_BIN_DIR "${CMAKE_INSTALL_PREFIX}/bin")
set(PROJECT_DATA_DIR "${CMAKE_INSTALL_PREFIX}/share/machinekit")
set(INCLUDE_DIR "${CMAKE_INSTALL_PREFIX}/include/machinekit")
set(PROJECT_LIB_DIR "${CMAKE_INSTALL_PREFIX}/lib")

set(REALTIME ${PROJECT_BIN_DIR}/realtime)
set(EMC2_BIN_DIR ${PROJECT_BIN_DIR})
set(EMC2_RTLIB_DIR ${PROJECT_LIBEXEC_DIR})
set(prefix ${PROJECT_BINARY_DIR})
set(exec_prefix ${PROJECT_BINARY_DIR})
set(EMC2_SYSTEM_CONFIG_DIR ${PROJECT_SYSCONF_DIR}/machinekit)
set(EMC2_LIBEXEC_DIR ${PROJECT_LIBEXEC_DIR})
set(EMC2_TCL_LIB_DIR "${CMAKE_INSTALL_PREFIX}/lib/tcltk/machinekit")

set(RUN_IN_PLACE "no")

configure_file("rtapi.in" "rtapi.ini.dist" @ONLY)
configure_file("halrun.in" "halrun.dist" @ONLY)
configure_file("realtime.in" "realtime.dist" @ONLY)
configure_file("machinekit.ini.tmpl.in" "machinekit.ini.dist" @ONLY)
configure_file("haltcl.in" "haltcl.dist" @ONLY)

configure_file("${CMAKE_SOURCE_DIR}/src/Makefile.in" "Makefile.modinc.dist" @ONLY)

install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/realtime.dist
    DESTINATION bin
    RENAME realtime)

install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/halrun.dist
    DESTINATION bin
    RENAME halrun)

install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/haltcl.dist
    DESTINATION bin
    RENAME haltcl)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/rtapi.ini.dist
    DESTINATION ${INSTALL_SYSCONF_DIR}/machinekit
    RENAME rtapi.ini)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/machinekit.ini.dist
    DESTINATION ${INSTALL_SYSCONF_DIR}/machinekit
    RENAME machinekit.ini)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/Makefile.modinc.dist
    DESTINATION ${PROJECT_DATA_DIR}
    RENAME Makefile.modinc)
