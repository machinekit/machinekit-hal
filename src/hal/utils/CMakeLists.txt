set(HALCMD_SRCS
    halcmd.c
    halcmd_commands.c
    halcmd_main.c
    halcmd_rtapiapp.cc
    halcmd_completion.c)

add_library(halcmd_rtapiapp OBJECT halcmd_rtapiapp.cc)
target_compile_options(halcmd_rtapiapp PUBLIC -fPIC)
add_dependencies(halcmd_rtapiapp npb_gen_files)

foreach(util comp instcomp)
    set(src ${CMAKE_CURRENT_SOURCE_DIR}/${util}.g)
    add_custom_command(
            OUTPUT ${util}.py
            COMMAND ${YAPPS} ${src} ${util}.py
            DEPENDS ${src}
            COMMENT "Creating ${util}.py")
    add_python_target(${util} ${PROJECT_BIN_DIR} ""
        ${CMAKE_CURRENT_BINARY_DIR} ${util})
    _install_script(${PROJECT_BIN_DIR}/${util})
endforeach()

add_executable(halcmd ${HALCMD_SRCS})
target_link_libraries(halcmd mkini mkhal mktalk-pb2++
    mtalk rtapi_math readline)
target_compile_definitions(halcmd PRIVATE ULAPI)
_install(halcmd)

add_library(tk_hal SHARED halsh.c halcmd.c halcmd_commands.c halcmd_rtapiapp.cc)
set_target_properties(tk_hal PROPERTIES
    COMPILE_DEFINITIONS "ULAPI"
    LIBRARY_OUTPUT_DIRECTORY ${PROJECT_TCL_DIR}
    OUTPUT_NAME hal
    PREFIX "")
set_property(TARGET tk_hal APPEND
    PROPERTY INCLUDE_DIRECTORIES "${TCL_INCLUDE_PATH}")
target_link_libraries(tk_hal mkhal mtalk mktalk-pb2++)
install(TARGETS tk_hal LIBRARY DESTINATION lib/tcltk/machinekit)

if(WITH_GTK)
    add_executable(halmeter meter.c miscgtk.c)
    set_target_properties(halmeter PROPERTIES COMPILE_DEFINITIONS "ULAPI")
    set_property(TARGET halmeter APPEND PROPERTY
        INCLUDE_DIRECTORIES "${GTK2_INCLUDE_DIRS}")
    target_link_libraries(halmeter mkhal ${GTK2_LIBRARIES})
    _install(halmeter)

    set(HALSCOPE_SRCS
        scope.c
        scope_horiz.c
        scope_vert.c
        scope_trig.c
        scope_disp.c
        scope_files.c
        miscgtk.c)

    add_executable(halscope ${HALSCOPE_SRCS})
    set_target_properties(halscope PROPERTIES COMPILE_DEFINITIONS "ULAPI")
    set_property(TARGET halscope APPEND PROPERTY
        INCLUDE_DIRECTORIES "${GTK2_INCLUDE_DIRS}")
    target_link_libraries(halscope mkhal rtapi_math ${GTK2_LIBRARIES})
    _install(halscope)
endif()

if(WITH_DRIVERS)
    add_executable(pci_write pci_write.c upci.c)
    _install(pci_write)

    add_executable(pci_read pci_read.c upci.c)
    _install(pci_read)
endif()

# fixme
# set_property(GLOBAL APPEND PROPERTY target-dev pyhal_utils)
