set(COMP_SRC mesa_7i65 mesa_uart pcl720 serport)

unset(_gen_c)
# generate c from comps
foreach(file ${COMP_SRC})
    _comp_to_c(${file})
endforeach()

# create targets from generated c files
foreach(file ${_gen_c})
    get_filename_component(name ${file} NAME_WE)
    _to_rtlib(${name} ${file})
endforeach()

unset(_gen_man)
# create manpages
foreach(file ${COMP_SRC})
    _comp_to_man(${file})
endforeach()

add_custom_target(man_drivers ALL DEPENDS ${_gen_man})
_install_man("${_gen_man}")

if(BUILD_DRIVERS)
    _to_rtlib(hal_parport hal_parport.c)
    _to_rtlib(pci_8255 pci_8255.c)
    _to_rtlib(hal_tiro hal_tiro.c)
    _to_rtlib(hal_stg hal_stg.c)
    _to_rtlib(hal_vti hal_vti.c)
    _to_rtlib(hal_motenc hal_motenc.c)
    _to_rtlib(hal_ax5214h hal_ax5214h.c)
    _to_rtlib(hal_speaker hal_speaker.c)
    _to_rtlib(hal_skeleton hal_skeleton.c)
    _to_rtlib(hal_p260c hal_p260c.c)
    _to_rtlib(probe_parport probe_parport.c)

    if(WITH_TARGET_PLATFORM_RASPBERRY)
        set(SRC hal_gpio.c cpuinfo.c)
        _to_rtlib(hal_gpio ${SRC})
        _to_rtlib(hal_spi hal_spi.c)
    endif()

    if(WITH_TARGET_PLATFORM_BEAGLEBONE)
        _to_rtlib(hal_bb_gpio hal_bb_gpio.c)
        _to_rtlib(hal_arm335xQEP hal_arm335xQEP.c)

        set(SRC hal_pru_generic/hal_pru_generic.c
            hal_pru_generic/pwmgen.c hal_pru_generic/stepgen.c
            hal_pru_generic/encoder.c
            $<TARGET_OBJECTS:prussdrv>)

        _to_rtlib(hal_pru_generic ${SRC})
        _to_rtlib(pepper hal_pru_generic/pepper.c)
    endif()

    if(WITH_TARGET_PLATFORM_ZEDBOARD)
        _to_rtlib(hal_zed_gpio hal_zed_gpio.c)
        _to_rtlib(hal_zed_can hal_zed_can.c)
    endif()

    if(WITH_TARGET_PLATFORM_H3)
        _to_rtlib(hal_gpio_h3 hal_gpio_h3.c)
    endif()

    if(WITH_TARGET_PLATFORM_CHIP)
        _to_rtlib(hal_chip_gpio hal_chip_gpio.c chip/mmap_gpio.c)
    endif()
endif()

if(WITH_USERMODE_PCI)
    add_subdirectory(mesa-hostmot2)
endif()

#[[
# Subdirectory: hal/drivers

ifdef TARGET_PLATFORM_SOCFPGA
ifeq ($(BUILD_SYS),user-dso)
obj-$(CONFIG_HM2_SOC_OL) += hm2_soc_ol.o
hm2_soc_ol-objs := hal/drivers/mesa-hostmot2/hm2_soc_ol.o
endif
endif



# hal_pru_generic driver
ifdef TARGET_PLATFORM_BEAGLEBONE
ifeq ($(BUILD_SYS),user-dso)
obj-m += hal_pru_generic.o
hal_pru_generic-objs :=                           \
    hal/drivers/hal_pru_generic/hal_pru_generic.o \
    hal/drivers/hal_pru_generic/pwmgen.o          \
    hal/drivers/hal_pru_generic/stepgen.o         \
    hal/drivers/hal_pru_generic/encoder.o         \
    hal/support/pru/prussdrv.o                    \
    $(LIBPTHREAD)
endif
endif

]]
