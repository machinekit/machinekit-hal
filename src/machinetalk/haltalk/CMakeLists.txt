add_executable(haltalk haltalk_group.cc haltalk_rcomp.cc haltalk_command.cc
        haltalk_introspect.cc haltalk_bridge.cc haltalk_main.cc)
target_link_libraries(haltalk mtalk linuxcnchal linuxcncini machinetalk-pb2++)
target_compile_definitions(haltalk PRIVATE ULAPI)
_install(haltalk)


#[[
add_executable(unionread unionread.c $<TARGET_OBJECTS:nanopb>)
target_compile_definitions(unionread PRIVATE PB_FIELD_32BIT)
_install(unionread)

foreach(file npbdecode rtprintf)
    add_executable(${file} ${file}.c)
    target_link_libraries(${file} machinetalk-npb)
    target_compile_definitions(${file} PRIVATE PB_FIELD_32BIT)
    _install(${file})
endforeach()

foreach(file position encdec linmove)
    add_executable(${file} ${file}.cc)
    target_link_libraries(${file} machinetalk-pb2++)
    target_compile_definitions(${file} PRIVATE PB_FIELD_32BIT)
    _install(${file})
endforeach()

target_link_libraries(encdec mtalk linuxcncini)
target_link_libraries(linmove mtalk linuxcncini)
]]
