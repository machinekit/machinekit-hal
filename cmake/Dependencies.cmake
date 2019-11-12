include(FindPkgConfig)
if(NOT PKG_CONFIG_FOUND)
    message(FATAL_ERROR "pkg-config not found: install pkg-config")
endif()

# check debian version
execute_process(COMMAND lsb_release -rs
    OUTPUT_VARIABLE RELEASE_NUMBER
    OUTPUT_STRIP_TRAILING_WHITESPACE)

if(RELEASE_NUMBER VERSION_GREATER "9.99")
    set(GT_STRETCH 1)
endif()

if(NOT CMAKE_VERSION VERSION_LESS "3.12")
    find_package(Python2 COMPONENTS Interpreter Development)
else()
    find_package(Python2_local)
endif()
set(HAVE_PYTHON ${Python2_VERSION})

if(NOT IS_DIRECTORY ${Python2_SITELIB}/google/protobuf)
    message(FATAL_ERROR "python-protobuf not found: install python-protobuf")
endif()

pkg_check_modules(AVAHI avahi-client)
if(NOT AVAHI_FOUND)
    message(FATAL_ERROR "avahi-client not found: install libavahi-client-dev")
endif()
set(HAVE_AVAHI 1)

find_program(CYTHON cython)
if(NOT CYTHON)
    message(FATAL_ERROR "cython not found: install cython")
endif()

find_package(Boost COMPONENTS python)
if(NOT Boost_FOUND)
    message(FATAL_ERROR "boost not found: install libboost-python-dev")
endif()
set(HAVE_BOOST 1)
set(HAVE_BOOST_PYTHON 1)

pkg_check_modules(LCG libcgroup)
if(NOT LCG_FOUND)
    message(FATAL_ERROR "libcgroup not found: install libcgroup-dev")
endif()
set(HAVE_LIBCGROUP 1)

pkg_check_modules(LCK ck)
if(LCK_FOUND)
    set(HAVE_CK 1)
endif()

pkg_check_modules(CZMQ libczmq>=4.0.2)
if(NOT CZMQ_FOUND)
    message(FATAL_ERROR "libczmq not found: install libczmq-dev")
endif()
set(HAVE_CZMQ 1)

if(WITH_GTK)
    find_package(GTK2)
    if(NOT GTK2_FOUND)
        message(FATAL_ERROR "libgtk2.0 not found: install libgtk2.0-dev")
    endif()
    set(HAVE_GTK 1)
endif()

pkg_check_modules(JANSSON jansson>=2.5)
if(NOT JANSSON_FOUND)
    message(FATAL_ERROR "jansson not found: install libjansson-dev")
endif()
set(HAVE_JANSSON 1)

include(FindTCL)
if(NOT TCL_FOUND)
    message(FATAL_ERROR "tcl not found: install tcl8.6-dev")
endif()

if(WITH_MODBUS)
    pkg_check_modules(MODBUS libmodbus>=3)
    if(NOT MODBUS_FOUND)
        message(FATAL_ERROR "libmodbus not found: install libmodbus-dev")
    endif()
endif()

if(CMAKE_VERSION VERSION_LESS "3.6")
    find_package(Protobuf)
    set(Protobuf_FOUND ${PROTOBUF_FOUND})
    set(Protobuf_INCLUDE_DIR ${PROTOBUF_INCLUDE_DIRS})
else()
    find_package(Protobuf 2.4.1)
endif()

if(NOT Protobuf_FOUND)
    message(FATAL_ERROR "protobuf not found: install libprotobuf-dev")
endif()
set(HAVE_PROTOBUF 1)

if(WITH_LIBUSB10)
    pkg_check_modules(LIBUSB libusb-1.0)
    if(NOT LIBUSB_FOUND)
        message(FATAL_ERROR "libusb not found: install libusb-1.0-0-dev")
    endif()
endif()

pkg_check_modules(LWS libwebsockets)
if(NOT LWS_FOUND)
    message(FATAL_ERROR "websockets not found: install libwebsockets-dev")
endif()
set(HAVE_LWS 1)

find_program(PROTOC protoc)
if(NOT PROTOC)
    message(FATAL_ERROR "protoc not found: install protobuf-compiler")
endif()

find_program(PATCHELF patchelf)
if(NOT PATCHELF)
    message(FATAL_ERROR "patchelf not found: install patchelf")
endif()

pkg_check_modules(UUID uuid)
if(NOT UUID_FOUND)
    message(FATAL_ERROR "uuid not found: install uuid-dev")
endif()
set(HAVE_UUID 1)

if(GT_STRETCH)
    find_program(YAPPS yapps2)
else()
    find_program(YAPPS yapps)
endif()
if(NOT YAPPS)
    message(FATAL_ERROR "yapps not found: install yapps2 package")
endif()

if(GT_STRETCH)
    if(NOT IS_DIRECTORY ${Python2_SITELIB}/yapps)
        message(FATAL_ERROR "python-yapps not found: install python-yapps")
    endif()
endif()

check_include_files(readline/readline.h HAVE_READLINE)
if(NOT HAVE_READLINE)
    message(FATAL_ERROR "readline not found: install libreadline-dev")
endif()

find_package(Threads REQUIRED)

find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif(CCACHE_FOUND)

if(WITH_USERMODE_PCI)
    pkg_check_modules(UDEV libudev)
    if(NOT UDEV_FOUND)
        message(FATAL_ERROR "udev not found: install libudev-dev")
    endif()
    set(HAVE_LIBUDEV 1)
endif()

find_program(PIDOF pidof)
if(NOT PIDOF)
    message(FATAL_ERROR "pidof not found: install sysvinit-utils")
endif()

find_program(PS ps)
find_program(AWK awk)

#[[
# required packages

# TODO libgl1-mesa-dev libglu1-mesa-dev tk-dev

pkg_check_modules(CZMQ REQUIRED libczmq>=2.2.0)
pkg_check_modules(AVAHI REQUIRED avahi-client)
pkg_check_modules(JANSSON REQUIRED jansson>=2.5)
pkg_check_modules(LWS REQUIRED libwebsockets)
pkg_check_modules(SSL REQUIRED libssl)
pkg_check_modules(URIPARSER REQUIRED liburiparser)
pkg_check_modules(UUID REQUIRED uuid)
find_package(Boost REQUIRED COMPONENTS python serialization thread)
find_package(GTK2 REQUIRED)
]]
