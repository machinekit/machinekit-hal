add_definitions(-DULAPI)

add_library(shmdrvapi OBJECT shmdrvapi.c)
add_library(mkshm SHARED $<TARGET_OBJECTS:shmdrvapi>)
target_link_libraries(mkshm rt)
set_target_properties(mkshm PROPERTIES VERSION ${GENERIC_LIB_VERSION})
_install(mkshm)

install(FILES shmdrv.rules
    DESTINATION /etc/udev/rules.d
    RENAME 50-shmdrv.rules)

install(FILES limits.d-machinekit.conf
    DESTINATION /etc/security/limits.d
    RENAME machinekit.conf)
