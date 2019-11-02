include_directories(${Python2_INCLUDE_DIRS})

#set(dep halfile.py launcher.py service.py nosetests)
set(dep nosetests)
add_custom_target(py_deps
    COMMAND ${CMAKE_COMMAND}
        -E make_directory ${PROJECT_PYTHON_DIR}/machinekit
    COMMAND ${CMAKE_COMMAND}
        -E touch ${PROJECT_PYTHON_DIR}/machinekit/__init__.py
    COMMAND cp -af ${dep} ${PROJECT_PYTHON_DIR}/machinekit
    DEPENDS ${dep}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

foreach(file hal rtapi compat shmcommon)
    add_custom_command(
            OUTPUT  ${CMAKE_CURRENT_BINARY_DIR}/${file}.c
            COMMAND ${CYTHON} -o ${CMAKE_CURRENT_BINARY_DIR}/${file}.c ${file}.pyx
            DEPENDS ${file}.pyx ${file}.pxd
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            COMMENT "Generate C files from ${file}.pyx")
    add_library(${file}_obj OBJECT ${CMAKE_CURRENT_BINARY_DIR}/${file}.c)
    target_compile_definitions(${file}_obj PRIVATE "ULAPI")
    add_dependencies(${file}_obj py_deps)
endforeach()

#configure_file("config.py.in" "${PROJECT_PYTHON_DIR}/machinekit/config.py" @ONLY)

# create files for installation
set(PROJECT_SYSCONF_DIR "${INSTALL_SYSCONF_DIR}")
set(PROJECT_LIBEXEC_DIR "${CMAKE_INSTALL_PREFIX}/lib/machinekit")
set(PROJECT_BINARY_DIR "${CMAKE_INSTALL_PREFIX}")
set(PROJECT_BIN_DIR "${CMAKE_INSTALL_PREFIX}/bin")

#configure_file("config.py.in" "config.py" @ONLY)
