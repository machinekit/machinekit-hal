add_executable(sizes sizes.c)
_install(sizes)

foreach(file encdec position linmove)
    add_executable(${file} ${file}.cc)
    target_link_libraries(${file} machinetalk-pb2++)
    target_compile_definitions(${file} PRIVATE PB_FIELD_32BIT)
    _install(${file})
endforeach()

target_link_libraries(encdec mtalk linuxcncini)
target_link_libraries(linmove mtalk linuxcncini)

add_executable(unionread unionread.c $<TARGET_OBJECTS:nanopb>)
target_compile_definitions(unionread PRIVATE PB_FIELD_32BIT)
_install(unionread)

foreach(file rtprintf npbdecode)
    add_executable(${file} ${file}.c)
    target_link_libraries(${file} machinetalk-npb)
    target_compile_definitions(${file} PRIVATE PB_FIELD_32BIT)
    _install(${file})
endforeach()
target_compile_definitions(rtprintf PRIVATE ULAPI)

#[[
foreach(util comp instcomp)
    set(src ${CMAKE_CURRENT_SOURCE_DIR}/${util}.g)
    add_custom_command(
        OUTPUT ${util}.py
        COMMAND ${YAPPS} ${src} ${util}.py
        DEPENDS ${src}
        COMMENT "Creating ${util}.py")
    add_python_target(${util} ${PROJECT_BIN_DIR} ""
        ${CMAKE_CURRENT_BINARY_DIR} ${util})
endforeach()
]]
