add_subdirectory(machinekit)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_PYTHON_DIR}/machinekit)

# fix rpath to enable RIP on build dir
SET(CMAKE_SKIP_BUILD_RPATH  FALSE)
SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
SET(CMAKE_INSTALL_RPATH "${_rpath}")
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

set(PWD ${CMAKE_CURRENT_BINARY_DIR}/machinekit)

add_library(cy_hal SHARED $<TARGET_OBJECTS:hal_obj>
    $<TARGET_OBJECTS:halcmd_rtapiapp>)
target_link_libraries(cy_hal mk_rip linuxcnchal mtalk)
set_target_properties(cy_hal PROPERTIES
    OUTPUT_NAME "hal"
    PREFIX "")
cleanup_rpath_hack(cy_hal)

add_library(cy_rtapi SHARED $<TARGET_OBJECTS:rtapi_obj>
    $<TARGET_OBJECTS:halcmd_rtapiapp>)
target_link_libraries(cy_rtapi mk_rip linuxcnchal mtalk machinetalk-pb2++)
set_target_properties(cy_rtapi PROPERTIES
    OUTPUT_NAME "rtapi"
    PREFIX "")
cleanup_rpath_hack(cy_rtapi)

add_library(cy_compat SHARED $<TARGET_OBJECTS:compat_obj>)
target_link_libraries(cy_compat mk_rip linuxcnchal)
set_target_properties(cy_compat PROPERTIES
    OUTPUT_NAME "compat"
    PREFIX "")
cleanup_rpath_hack(cy_compat)

add_library(cy_shmcommon SHARED $<TARGET_OBJECTS:shmcommon_obj>
    $<TARGET_OBJECTS:shmdrvapi>)
target_link_libraries(cy_shmcommon mk_rip)
set_target_properties(cy_shmcommon PROPERTIES
    OUTPUT_NAME "shmcommon"
    PREFIX "")
cleanup_rpath_hack(cy_shmcommon)

_install_py(machinekit cy_hal cy_rtapi cy_compat cy_shmcommon)
