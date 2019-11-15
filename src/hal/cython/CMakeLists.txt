add_subdirectory(machinekit)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_PYTHON_DIR}/machinekit)

set(PWD ${CMAKE_CURRENT_BINARY_DIR}/machinekit)

add_library(cy_hal SHARED $<TARGET_OBJECTS:hal_obj>
    $<TARGET_OBJECTS:halcmd_rtapiapp>)
target_link_libraries(cy_hal mkhal mtalk)
set_target_properties(cy_hal PROPERTIES
    OUTPUT_NAME "hal"
    PREFIX "")

add_library(cy_rtapi SHARED $<TARGET_OBJECTS:rtapi_obj>
    $<TARGET_OBJECTS:halcmd_rtapiapp>)
target_link_libraries(cy_rtapi mkhal mtalk mktalk-pb2++)
set_target_properties(cy_rtapi PROPERTIES
    OUTPUT_NAME "rtapi"
    PREFIX "")

add_library(cy_compat SHARED $<TARGET_OBJECTS:compat_obj>)
target_link_libraries(cy_compat mkhal)
set_target_properties(cy_compat PROPERTIES
    OUTPUT_NAME "compat"
    PREFIX "")

add_library(cy_shmcommon SHARED $<TARGET_OBJECTS:shmcommon_obj>
    $<TARGET_OBJECTS:shmdrvapi>)
set_target_properties(cy_shmcommon PROPERTIES
    OUTPUT_NAME "shmcommon"
    PREFIX "")
