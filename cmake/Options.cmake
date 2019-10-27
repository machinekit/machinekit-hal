include(CheckIncludeFiles)
include(CheckFunctionExists)

option(WITH_EMCWEB "Build the emcweb interface" OFF)
option(WITH_USERMODE_PCI "Build PCI drivers with usermode PCI support" ON)
option(WITH_DRIVERS "Build hardware drivers" ON)
option(WITH_DOCUMENTATION "Build asciidoc documentation" OFF)
option(WITH_XENOMAI "Build Xenomai userland threads" ON)
option(WITH_RT_PREEMPT "Build RT_PREEMPT threads modules" ON)
option(WITH_POSIX "Build POSIX non-realtime threads modules" ON)
option(WITH_MODBUS "Build drivers that use libmodbus" ON)
option(WITH_LIBUSB10 "Build drivers that use libusb-1.0" ON)
option(WITH_GTK "Enable the parts of Machinekit that depend on GTK" ON)
option(WITH_PYTHON "Enable the parts of Machinekit that depend on Python" ON)
option(WITH_DEV "Enable unstable development code" OFF)

option(WITH_PC "Build for PC platform (default)" ON)
option(WITH_BEAGLEBONE "Build for Beaglebone platform" OFF)
option(WITH_CHIP "Build for Chip platform" OFF)
option(WITH_H3 "Build for H3 SoC platform" OFF)
option(WITH_RASPBERRY "Build for Raspberry" OFF)
option(WITH_SOCFPGA "Build for Socfpga platform" OFF)
option(WITH_ZEDBOARD "Build for Zedboard platform" OFF)

option(USE_DH_PYTHON "Use dh_python2 to package the python modules" OFF)

if(WITH_PC)
    set(TARGET_PLATFORM_PC 1)
elseif(WITH_BEAGLEBONE)
    set(TARGET_PLATFORM_BEAGLEBONE 1)
elseif(WITH_CHIP)
    set(TARGET_PLATFORM_CHIP 1)
elseif(WITH_H3)
    set(TARGET_PLATFORM_H3 1)
elseif(WITH_RASPBERRY)
    set(TARGET_PLATFORM_RASPBERRY 1)
elseif(WITH_SOCFPGA)
    set(TARGET_PLATFORM_SOCFPGA 1)
elseif(WITH_ZEDBOARD)
    set(TARGET_PLATFORM_ZEDBOARD 1)
endif()

include(Dependencies)

if(WITH_EMCWEB)
    set(BUILD_EMCWEB 1)
endif()

if(WITH_USERMODE_PCI)
    set(USERMODE_PCI 1)
endif()

if(WITH_DRIVERS)
    set(BUILD_DRIVERS 1)
endif()

if(WITH_RT_PREEMPT)
    set(HAVE_RT_PREEMPT_THREADS 1)
    set(BUILD_THREAD_FLAVORS rt_preempt)
endif()

if(WITH_POSIX)
    set(HAVE_POSIX_THREADS 1)
    set(BUILD_THREAD_FLAVORS ${BUILD_THREAD_FLAVORS} posix)
endif()

find_program(XENO_CONFIG xeno-config)
if(NOT XENO_CONFIG OR NOT WITH_XENOMAI)
    message(STATUS "xenomai flavor disabled")
    set(XENOMAI_SKIN native)
else()
    set(BUILD_THREAD_FLAVORS ${BUILD_THREAD_FLAVORS} xenomai)
    set(HAVE_XENOMAI_THREADS 1)
    execute_process(COMMAND ${XENO_CONFIG} --version
            OUTPUT_VARIABLE XENO_VERSION
            OUTPUT_STRIP_TRAILING_WHITESPACE)
    if(XENO_VERSION MATCHES "^2.*")
        set(XENOMAI_SKIN native)
        set(XENOMAI_V2 1)
    else()
        set(XENOMAI_SKIN alchemy)
    endif()
    execute_process(COMMAND ${XENO_CONFIG} --skin ${XENOMAI_SKIN} --cflags
            OUTPUT_VARIABLE XENO_CFLAGS
            OUTPUT_STRIP_TRAILING_WHITESPACE)
    execute_process(COMMAND ${XENO_CONFIG} --skin ${XENOMAI_SKIN} --ldflags
            OUTPUT_VARIABLE XENO_LDFLAGS
            OUTPUT_STRIP_TRAILING_WHITESPACE)
endif()

set(RTAPI_POSIX_ID 0)
set(RTAPI_POSIX_NAME "posix")
set(POSIX_THREADS_SOURCE "rt-preempt.c")
set(POSIX_THREADS_RTFLAGS "-pthread")
set(POSIX_THREADS_LDFLAGS "-lpthread -lrt")
set(RTAPI_RT_PREEMPT_ID 1)
set(RTAPI_RT_PREEMPT_NAME "rt-preempt")
set(RT_PREEMPT_THREADS_SOURCE "rt-preempt.c")
set(RT_PREEMPT_THREADS_RTFLAGS "-pthread")
set(RT_PREEMPT_THREADS_LDFLAGS "-lpthread -lrt")
set(RTAPI_XENOMAI_ID 2)
set(RTAPI_XENOMAI_NAME "xenomai")
set(XENOMAI_THREADS_SOURCE "xenomai.c")
set(XENOMAI_THREADS_RTFLAGS "${XENO_CFLAGS}")
set(XENOMAI_THREADS_LDFLAGS "${XENO_LDFLAGS}")
set(RTAPI_XENOMAI_KERNEL_ID 3)
set(RTAPI_XENOMAI_KERNEL_NAME "xenomai-kernel")
set(RTAPI_RTAI_KERNEL_ID 4)
set(RTAPI_RTAI_KERNEL_NAME "rtai-kernel")

set(RUNDIR "/tmp")
set(EMC2_RTLIB_DIR ${PROJECT_LIBEXEC_DIR})
set(EMC2_SYSTEM_CONFIG_DIR "/etc/machinekit")
set(EMC2_PO_DIR "${PROJECT_LOCALE_DIR}")
set(EMC2_BIN_DIR "${PROJECT_BIN_DIR}")

# enable runtime comparison of GIT_CONFIG_SHA (config.h) and GIT_BUILD_SHA
find_program(GIT git)
if(GIT)
    execute_process(COMMAND git rev-parse --short HEAD
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE _out
        ERROR_QUIET
        OUTPUT_STRIP_TRAILING_WHITESPACE)
    if(NOT _out)
        set(_out "non_git_repo")
        set(GIT_CONFIG_SHA ".")
    else()
        set(GIT_CONFIG_SHA ".${_out}")
        execute_process(COMMAND git rev-parse --abbrev-ref HEAD
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            OUTPUT_VARIABLE _branch
            ERROR_QUIET
            OUTPUT_STRIP_TRAILING_WHITESPACE)
    endif()
else()
    set(_out "no_git_found")
    set(GIT_CONFIG_SHA ".")
endif()
if(NOT _branch)
    set(_branch "none")
endif()

set(PACKAGE_VERSION "${MK_VERSION_MAJOR}.${MK_VERSION_MINOR}")
set(GIT_VERSION "v${PACKAGE_VERSION}~${_branch}~${_out}")
set(GIT_CONFIG_SHA "${_out}")
set(GENERIC_LIB_VERSION "${PACKAGE_VERSION}")

# populate config.h variables
check_include_files(linux/hidraw.h HIDRAW_H_USABLE)
check_include_files(stdint.h HAVE_STDINT_H)
check_include_files(stdlib.h HAVE_STDLIB_H)
check_include_files(strings.h HAVE_STRINGS_H)
check_include_files(string.h HAVE_STRING_H)
check_include_files(backtrace.h HAVE_BACKTRACE_H)
check_include_files(GL/glu.h HAVE_GL_GLU_H)
check_include_files(GL/gl.h HAVE_GL_GL_H)
check_include_files(inttypes.h HAVE_INTTYPES_H)
check_include_files(libbacktrace/backtrace.h HAVE_LIBBACKTRACE_BACKTRACE_H)
check_include_files(libintl.h HAVE_LIBINTL_H)
check_include_files(locale.h HAVE_LOCALE_H)
check_include_files(memory.h HAVE_MEMORY_H)
check_include_files(ncurses.h HAVE_NCURSES_H)
check_include_files(readline/history.h HAVE_READLINE_HISTORY_H)
set(HAVE_READLINE_READLINE_H ${HAVE_READLINE})
check_include_files(sys/stat.h HAVE_SYS_STAT_H)
check_include_files(sys/types.h HAVE_SYS_TYPES_H)
check_include_files(sys/wait.h HAVE_SYS_WAIT_H)
check_include_files(unistd.h HAVE_UNISTD_H)
check_include_files(X11/extensions/Xinerama.h HAVE_X11_EXTENSIONS_XINERAMA_H)
check_include_files(X11/Xaw/XawInit.h HAVE_X11_XAW_XAWINIT_H)
check_include_files(X11/Xmu/Xmu.h HAVE_X11_XMU_XMU_H)

set(CMAKE_REQUIRED_LIBRARIES m)
check_function_exists(sincos HAVE_SINCOS)
check_function_exists(__sincos HAVE___SINCOS)

# create config.h
configure_file(
    "${PROJECT_SOURCE_DIR}/src/config.in"
    "${PROJECT_BINARY_DIR}/include/config.h"
    @ONLY)
