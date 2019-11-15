foreach(file streamer sampler)
    add_executable(hal${file} ${file}_usr.c)
    target_link_libraries(hal${file} mkhal)
    target_compile_definitions(hal${file} PRIVATE "ULAPI")
    _install(hal${file})
endforeach()

set(files
    bldc.comp
    logic.comp
    rtfault.comp
    rtmon.comp
    thc.comp
    thcud.comp
    vari_bit_adc.comp)

if(WITH_USERMODE_PCI)
    set(files ${files} mesa_pktgyro_test.comp)
endif()

unset(_gen_man)
# create manpages
foreach(file ${_gen_comp})
    get_filename_component(name ${file} NAME_WE)
    _comp_to_man(${name} ${CMAKE_CURRENT_BINARY_DIR})
endforeach()
add_custom_target(man_conv ALL DEPENDS ${_gen_man})
_install_man("${_gen_man}")

unset(_gen_man)
# create manpages
foreach(file ${files})
    get_filename_component(name ${file} NAME_WE)
    _comp_to_man(${name})
endforeach()
add_custom_target(man_components ALL DEPENDS ${_gen_man})
_install_man("${_gen_man}")

unset(_gen_c)
# generate c from comps
foreach(file ${_gen_comp})
    get_filename_component(name ${file} NAME_WE)
    _comp_to_c(${name} ${CMAKE_CURRENT_BINARY_DIR})
endforeach()

# generate c from comps
foreach(file ${files})
    get_filename_component(name ${file} NAME_WE)
    _comp_to_c(${name})
endforeach()

# create targets from generated c files
foreach(file ${_gen_c})
    get_filename_component(name ${file} NAME_WE)
    _to_rtlib(${name}.comp ${file})
endforeach()

set(CMP_SRC
    counter
    delayline
    encoder_ratio
    encoder_ratiov2
    encoder
    encoderv2
    lcd
    matrix_kb
    mux_generic
    pepper
    pwmgen
    pwmgenv2
    sampler
    siggen
    sim_encoder
    stepgen
    stepgenv2
    streamer
    threads)

foreach(file ${CMP_SRC})
    _to_rtlib(${file} ${file}.c)
endforeach()

if(WITH_TARGET_PLATFORM_BEAGLEBONE)
    set(SRC hal_pru hal_prudebug)
    foreach(file ${SRC})
            set(_src ${file}.c $<TARGET_OBJECTS:prussdrv>)
            _to_rtlib(${file} ${_src})
    endforeach()
    _to_rtlib(pepper pepper.c)
endif()
