
unset(_gen_c)
_comp_to_c(adxl345)
_comp_to_c(scounter)

foreach(file ${_gen_c})
    get_filename_component(name ${file} NAME_WE)
    add_executable(${name} ${file})
    target_compile_definitions(${name} PRIVATE "ULAPI")
    target_link_libraries(${name} mkhal)
    _install(${name})
endforeach()
target_link_libraries(adxl345 rtapi_math)

if(WITH_MODBUS)
    add_executable(gs2_vfd gs2_vfd.c)
    target_compile_definitions(gs2_vfd PRIVATE "ULAPI")
    target_compile_options(gs2_vfd PRIVATE "${MODBUS_CFLAGS}")
    target_link_libraries(gs2_vfd mkhal ${MODBUS_LDFLAGS})
    _install(gs2_vfd)
endif()

if(HIDRAW_H_USABLE)
    add_executable(shuttlexpress shuttlexpress.c)
    target_compile_definitions(shuttlexpress PRIVATE "ULAPI")
    target_link_libraries(shuttlexpress mkhal)
    _install(shuttlexpress)
endif()

if(WITH_LIBUSB10)
    add_executable(xhc-hb04 xhc-hb04.cc)
    target_compile_definitions(xhc-hb04 PRIVATE "ULAPI")
    target_compile_options(xhc-hb04 PRIVATE "${LIBUSB_CFLAGS}")
    target_link_libraries(xhc-hb04 mkhal mkini rtapi_math ${LIBUSB_LDFLAGS})
    _install(xhc-hb04)

    add_executable(xhc-whb04b-6 xhc-whb04b-6/hal.cc xhc-whb04b-6/usb.cc
            xhc-whb04b-6/pendant-types.cc xhc-whb04b-6/pendant.cc
            xhc-whb04b-6/xhc-whb04b6.cc xhc-whb04b-6/main.cc)
    target_compile_definitions(xhc-whb04b-6 PRIVATE "ULAPI")
    target_compile_options(xhc-whb04b-6 PRIVATE "${LIBUSB_CFLAGS}")
    target_link_libraries(xhc-whb04b-6 mkhal rtapi_math ${LIBUSB_LDFLAGS})
    _install(xhc-whb04b-6)
endif()

add_executable(hy_vfd huanyang-vfd/hy_vfd.c huanyang-vfd/hy_comm.c)
target_compile_definitions(hy_vfd PRIVATE "ULAPI")
target_link_libraries(hy_vfd mkhal)
_install(hy_vfd)

if(WITH_PYTHON)
    set(USER_COMP_PY
        pyvcp
        hal_input
        hal_gpio_mcp23017
        hal_pwm_pca9685
        hal_storage
        hal_temp_ads7828
        hal_temp_bbb
        hal_temp_atlas
        hal_gy86
        gladevcp)

    foreach(file ${USER_COMP_PY})
        add_python_target(${file} ${PROJECT_BIN_DIR} ""
            ${CMAKE_CURRENT_SOURCE_DIR} ${file})
        _install_script(${PROJECT_BIN_DIR}/${file})
    endforeach()

    set(VISMACH_PY
        pumagui
        puma560gui
        rostock
        scaragui
        hexagui
        5axisgui
        max5gui
        maho600gui
        hbmgui)

    foreach(file ${VISMACH_PY})
        add_python_target(${file} ${PROJECT_BIN_DIR} ""
            ${CMAKE_CURRENT_SOURCE_DIR}/vismach ${file})
        _install_script(${PROJECT_BIN_DIR}/${file})
    endforeach()
endif()
