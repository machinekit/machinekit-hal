include_directories(${Python2_INCLUDE_DIRS})

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
