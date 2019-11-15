add_subdirectory(nanopb)
add_subdirectory(proto)
add_subdirectory(lib)
add_subdirectory(support)
add_subdirectory(haltalk)
add_subdirectory(msgcomponents)

if(WITH_PYTHON)
    add_python_target(pb2json ${PROJECT_PYTHON_DIR} .py
        ${CMAKE_CURRENT_SOURCE_DIR}/support pb2json)
    add_python_target(configserver ${PROJECT_BIN_DIR} ""
        ${CMAKE_CURRENT_SOURCE_DIR}/config-service configserver)
    _install_script(${PROJECT_BIN_DIR}/configserver)
    add_python_target(mkwrapper ${PROJECT_BIN_DIR} ""
        ${CMAKE_CURRENT_SOURCE_DIR}/mkwrapper mkwrapper)
    _install_script(${PROJECT_BIN_DIR}/mkwrapper)
    add_python_target(mklauncher ${PROJECT_BIN_DIR} ""
        ${CMAKE_CURRENT_SOURCE_DIR}/mklauncher mklauncher)
    _install_script(${PROJECT_BIN_DIR}/mklauncher)
    add_python_target(videoserver ${PROJECT_BIN_DIR} ""
        ${CMAKE_CURRENT_SOURCE_DIR}/videoserver videoserver)
    _install_script(${PROJECT_BIN_DIR}/videoserver)
endif()

if(WITH_DEV)
    add_executable(messagebus messagebus/messagebus.cc messagebus/rtproxy.cc)
    target_link_libraries(messagebus mtalk mkini mkhal 
                            linuxcnc-pb2++ linuxcnc-npb)
    target_compile_definitions(messagebus PRIVATE ULAPI)
endif()
