set(files
    hal/lib/config_module.h
    hal/lib/hal.h
    hal/lib/hal_accessor.h
    hal/lib/hal_accessor_macros.h
    hal/lib/hal_group.h
    hal/lib/hal_internal.h
    hal/lib/hal_list.h
    hal/lib/hal_logging.h
    hal/lib/hal_object.h
    hal/lib/hal_object_selectors.h
    hal/lib/hal_parport.h
    hal/lib/hal_priv.h
    hal/lib/hal_rcomp.h
    hal/lib/hal_ring.h
    hal/lib/hal_types.h
    hal/utils/halcmd_rtapiapp.h
    libnml/inifile/inifile.h
    libnml/inifile/inifile.hh
    rtapi/multiframe.h
    rtapi/multiframe_flag.h
    rtapi/ring.h
    rtapi/rtapi.h
    rtapi/rtapi_app.h
    rtapi/rtapi_atomics.h
    rtapi/rtapi_bitops.h
    rtapi/rtapi_byteorder.h
    rtapi/rtapi_common.h
    rtapi/rtapi_ctype.h
    rtapi/rtapi_errno.h
    rtapi/rtapi_exception.h
    rtapi/rtapi_export.h
    rtapi/rtapi_global.h
    rtapi/rtapi_heap.h
    rtapi/rtapi_heap_private.h
    rtapi/rtapi_hexdump.h
    rtapi/rtapi_int.h
    rtapi/rtapi_io.h
    rtapi/rtapi_math.h
    rtapi/rtapi_math64.h
    rtapi/rtapi_pci.h
    rtapi/rtapi_shmkeys.h
    rtapi/rtapi_string.h
    rtapi/shmdrv/shmdrv.h
    rtapi/rt-preempt.h
    rtapi/rtapi_compat.h
    rtapi/triple-buffer.h
    rtapi/xenomai.h
    machinetalk/include/pb-machinekit.h
    machinetalk/include/syslog_async.h
    machinetalk/include/czmq-watch.h
    machinetalk/include/inihelp.hh
    machinetalk/include/json2pb.hh
    machinetalk/include/bin2ascii.hh
    machinetalk/include/ll-zeroconf.hh
    machinetalk/include/mk-zeroconf-types.h
    machinetalk/include/mk-backtrace.h
    machinetalk/include/mk-service.hh
    machinetalk/include/mk-zeroconf.hh
    machinetalk/include/pbutil.hh
    machinetalk/include/setup_signals.h
    machinetalk/include/container.h
    machinetalk/include/halpb.hh)

execute_process(COMMAND ${CMAKE_COMMAND} -E 
                    make_directory ${INCLUDE_DIR}/protobuf)

unset(_headers)
foreach(file ${files})
    get_filename_component(name ${file} NAME)
    execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink
                        ${CMAKE_SOURCE_DIR}/src/${file} ${INCLUDE_DIR}/${name})
    set(_headers ${_headers} ${INCLUDE_DIR}/${name})
endforeach()

foreach(file pb.h pb_decode.h pb_encode.h)
    execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink
                        ${CMAKE_SOURCE_DIR}/src/machinetalk/nanopb/${file}
                        ${INCLUDE_DIR}/protobuf/${file})
    set(_headers ${_headers} ${INCLUDE_DIR}/protobuf/${file})
endforeach()

if(WITH_USERMODE_PCI)
    set(mesa_files
        hal/drivers/mesa-hostmot2/bitfile.h
        hal/drivers/mesa-hostmot2/hm2_pci.h
        hal/drivers/mesa-hostmot2/hostmot2.h
        hal/drivers/mesa-hostmot2/hm2_7i43.h
        hal/drivers/mesa-hostmot2/hm2_soc_ol.h
        hal/drivers/mesa-hostmot2/lbp16.h
        hal/drivers/mesa-hostmot2/hm2_7i90.h
        hal/drivers/mesa-hostmot2/hm2_test.h
        hal/drivers/mesa-hostmot2/sserial.h
        hal/drivers/mesa-hostmot2/hm2_eth.h
        hal/drivers/mesa-hostmot2/hostmot2-lowlevel.h)

    execute_process(COMMAND ${CMAKE_COMMAND} -E 
                        make_directory ${INCLUDE_DIR}/mesa
                    COMMAND ${CMAKE_COMMAND} -E 
                        make_directory ${INCLUDE_DIR}/userpci)

    foreach(file ${mesa_files})
        get_filename_component(name ${file} NAME)
        execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink
                            ${CMAKE_SOURCE_DIR}/src/${file} 
                            ${INCLUDE_DIR}/mesa/${name})
        set(_headers ${_headers} ${INCLUDE_DIR}/mesa/${name})
    endforeach()

    set(uspci_files
        rtapi/userpci/device.h
        rtapi/userpci/firmware.h
        rtapi/userpci/gfp.h
        rtapi/userpci/list.h
        rtapi/userpci/module.h
        rtapi/userpci/slab.h
        rtapi/userpci/string.h)

    foreach(file ${uspci_files})
        get_filename_component(name ${file} NAME)
        execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink
                            ${CMAKE_SOURCE_DIR}/src/${file} 
                            ${INCLUDE_DIR}/userpci/${name})
        set(_headers ${_headers} ${INCLUDE_DIR}/userpci/${name})
    endforeach()
endif()
