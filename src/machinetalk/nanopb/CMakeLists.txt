foreach(tool nanopb_generator.py protoc-gen-nanopb)
    add_custom_command(
            OUTPUT ${PROJECT_BIN_DIR}/${tool}
            COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_CURRENT_SOURCE_DIR}/generator/${tool}
            ${PROJECT_BIN_DIR}/${tool}
            DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/generator/${tool})
    add_custom_target(${tool} DEPENDS ${PROJECT_BIN_DIR}/${tool})
endforeach()

add_library(nanopb OBJECT pb_common.c pb_encode.c pb_decode.c)
add_dependencies(nanopb npb_gen_files)

set(PROTOC_GEN_NANOPB
    ${PROJECT_BIN_DIR}/protoc-gen-nanopb PARENT_SCOPE)
set(NANOPB_OPTIONS ${CMAKE_CURRENT_SOURCE_DIR}/../nanopb.options PARENT_SCOPE)
