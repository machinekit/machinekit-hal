_to_rtlib(hm2_eth hm2_eth.c)

set(SRC hm2_7i90 hm2_7i43 hm2_pci hm2_test)
foreach(file ${SRC})
    set(_src ${file}.c bitfile.c)
    _to_rtlib(${file} ${_src})
endforeach()

set(hostmot2_src
    abs_encoder.c
    backported-strings.c
    bitfile.c
    bspi.c
    dpll.c
    encoder.c
    hostmot2.c
    ioport.c
    led.c
    pins.c
    pwmgen.c
    raw.c
    resolver.c
    sserial.c
    stepgen.c
    tp_pwmgen.c
    tram.c
    uart.c
    watchdog.c)

_to_rtlib(hostmot2 ${hostmot2_src})
_to_rtlib(setsserial setsserial.c)
