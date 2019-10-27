add_subdirectory(nanopb)
add_subdirectory(proto)
add_subdirectory(lib)
add_subdirectory(support)
add_subdirectory(haltalk)

if(WITH_PYTHON)
    add_python_target(pb2json ${PROJECT_PYTHON_DIR} .py
        ${CMAKE_CURRENT_SOURCE_DIR}/support pb2json)
    add_python_target(configserver ${PROJECT_BIN_DIR} ""
        ${CMAKE_CURRENT_SOURCE_DIR}/config-service configserver)
    add_python_target(mkwrapper ${PROJECT_BIN_DIR} ""
        ${CMAKE_CURRENT_SOURCE_DIR}/mkwrapper mkwrapper)
    add_python_target(mklauncher ${PROJECT_BIN_DIR} ""
        ${CMAKE_CURRENT_SOURCE_DIR}/mklauncher mklauncher)
    add_python_target(videoserver ${PROJECT_BIN_DIR} ""
        ${CMAKE_CURRENT_SOURCE_DIR}/videoserver videoserver)
endif()

add_subdirectory(msgcomponents)
