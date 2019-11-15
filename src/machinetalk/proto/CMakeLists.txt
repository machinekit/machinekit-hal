set(MK_PROTO_SRC
    canon
    config
    emcclass
    firmware
    jplan
    log
    message
    motcmds
    nanopb
    object
    preview
    ros
    rtapi_message
    rtapicommand
    sample
    status
    task
    test
    types
    value)

set(NPB_DEFINITIONS PB_FIELD_32BIT PB_SYSTEM_HEADER=<pb-machinekit.h>)

set(PB_PROTO_PATHS --proto_path=${Protobuf_INCLUDE_DIR} --proto_path=src/machinetalk)

set(PB_GEN_DIR ${CMAKE_CURRENT_BINARY_DIR})
set(PB2_GEN_DIR ${PROJECT_PYTHON_DIR}/machinekit)

file(MAKE_DIRECTORY ${PB2_GEN_DIR})
file(MAKE_DIRECTORY ${INCLUDE_DIR}/protobuf)

# create proto C++
set(PB_GEN_OPTS ${PB_PROTO_PATHS} --cpp_out=${PB_GEN_DIR})

foreach(file ${MK_PROTO_SRC})
    set(out ${file}.pb.cc)
    set(PB_GEN ${PB_GEN} ${PB_GEN_DIR}/protobuf/${out})
    add_custom_command(
        OUTPUT  ${PB_GEN_DIR}/protobuf/${out}
        COMMAND ${PROTOC} ${PB_GEN_OPTS} src/machinetalk/protobuf/${file}.proto
        COMMAND ln -f ${PB_GEN_DIR}/protobuf/${file}.pb.h
                      ${INCLUDE_DIR}/protobuf/${file}.pb.h
        DEPENDS src/machinetalk/protobuf/${file}.proto
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Generate cpp files from src/machinetalk/protobuf/${file}.proto")
    add_custom_target(${file}.pb.h DEPENDS ${PB_GEN_DIR}/prof/${out})
endforeach()

add_custom_target(pb_gen_files ALL DEPENDS ${PB_GEN})

# create python files
set(PB2_GEN_OPTS ${PB_PROTO_PATHS} --python_out=${PB2_GEN_DIR})

foreach(file ${MK_PROTO_SRC})
    set(out ${file}_pb2.py)
    set(PB2_GEN ${PB2_GEN} ${PB2_GEN_DIR}/protobuf/${out})
    add_custom_command(
        OUTPUT ${PB2_GEN_DIR}/protobuf/${out}
        COMMAND ${PROTOC} ${PB2_GEN_OPTS} src/machinetalk/protobuf/${file}.proto
        DEPENDS src/machinetalk/protobuf/${file}.proto
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Generate python files from src/machinetalk/protobuf/${file}.proto")
    add_custom_target(${out} DEPENDS ${PB2_GEN_DIR}/protobuf/${out})
endforeach()

# create nanopb files
set(NPB_GEN_OPTS "${PB_PROTO_PATHS}"
    "--plugin=protoc-gen-nanopb=${PROTOC_GEN_NANOPB}"
    "--nanopb_out=\"--options-file=${NANOPB_OPTIONS}"
    "--library-include-format='#include" "<protobuf/%s>'"
    "--extension=.npb\":${PB_GEN_DIR}")

foreach(file ${MK_PROTO_SRC})
    set(out ${file}.npb.c)
    set(NPB_GEN ${NPB_GEN} ${PB_GEN_DIR}/protobuf/${out})
    add_custom_command(
        OUTPUT  ${PB_GEN_DIR}/protobuf/${out}
        COMMAND PYTHONPATH=${PROJECT_PYTHON_DIR}/machinekit/protobuf
            ${PROTOC} ${NPB_GEN_OPTS} src/machinetalk/protobuf/${file}.proto
        COMMAND ln -f ${PB_GEN_DIR}/protobuf/${file}.npb.h
                      ${INCLUDE_DIR}/protobuf/${file}.npb.h
        DEPENDS src/machinetalk/protobuf/${file}.proto
            protoc-gen-nanopb nanopb_generator.py ${PB2_GEN}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Generate npb cpp files from src/machinetalk/protobuf/${file}.proto")
    add_custom_target(${file}.npb.h DEPENDS ${PB_GEN_DIR}/protobuf/${out})
endforeach()

add_custom_target(npb_gen_files DEPENDS ${NPB_GEN})

add_library(mktalk-pb2++ SHARED ${PB_GEN})
set_target_properties(mktalk-pb2++ PROPERTIES COMPILE_FLAGS "${PROTOBUF_CFLAGS}")
target_link_libraries(mktalk-pb2++ ${PROTOBUF_LIBRARIES})
set_target_properties(mktalk-pb2++ PROPERTIES VERSION ${GENERIC_LIB_VERSION})
_install(mktalk-pb2++)

foreach(file ${MK_PROTO_SRC})
    if(NOT ${file} STREQUAL "nanopb")
            set(NPB_SRC ${NPB_SRC} ${PB_GEN_DIR}/protobuf/${file}.npb.c)
    endif()
endforeach()

add_library(npb-obj OBJECT ${NPB_SRC})
set_target_properties(npb-obj PROPERTIES COMPILE_DEFINITIONS "${NPB_DEFINITIONS}")
add_dependencies(npb-obj nanopb.npb.h)

add_library(mktalk-npb SHARED $<TARGET_OBJECTS:nanopb> $<TARGET_OBJECTS:npb-obj>
    ${PB_GEN_DIR}/protobuf/nanopb.npb.c)
set_target_properties(mktalk-npb PROPERTIES COMPILE_DEFINITIONS "${NPB_DEFINITIONS}")
set_target_properties(mktalk-npb PROPERTIES VERSION ${GENERIC_LIB_VERSION})
_install(mktalk-npb)

add_dependencies(mktalk-pb2++ mktalk-npb)
