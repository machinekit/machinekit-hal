add_executable(haltalk haltalk_group.cc haltalk_rcomp.cc haltalk_command.cc
        haltalk_introspect.cc haltalk_bridge.cc haltalk_main.cc)
target_link_libraries(haltalk mtalk mkhal mkini mktalk-pb2++)
target_compile_definitions(haltalk PRIVATE ULAPI)
_install(haltalk)
