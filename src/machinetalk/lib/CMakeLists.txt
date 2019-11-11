set(LIBMTALK_SRCS
    czmq-watch.c
    inihelp.cc
    json2pb.cc
    ll_zeroconf_register.cc
    mk_backtrace.c
    mk_service.cc
    mk_zeroconf.cc
    pbutil.cc
    setup_signals.c
    zeroconf_resolve.cc
)

add_library(syslog_async OBJECT syslog_async.c)

add_library(mtalk SHARED ${LIBMTALK_SRCS} $<TARGET_OBJECTS:syslog_async>)
target_link_libraries(mtalk ${AVAHI_LDFLAGS} ${CZMQ_LDFLAGS} ${JANSSON_LDFLAGS} 
                            ${UUID_LDFLAGS} ${Protobuf_LIBRARY_RELEASE})
if(GT_STRETCH)
    target_link_libraries(mtalk "-lzmq")
endif()
set_target_properties(mtalk PROPERTIES COMPILE_FLAGS ${UUID_CFLAGS})
set_target_properties(mtalk PROPERTIES VERSION ${GENERIC_LIB_VERSION})
_install(mtalk)
