add_definitions(-DULAPI)

add_library(shmdrvapi OBJECT shmdrvapi.c)
add_library(linuxcncshm SHARED $<TARGET_OBJECTS:shmdrvapi>)
target_link_libraries(linuxcncshm rt)
set_target_properties(linuxcncshm PROPERTIES VERSION ${GENERIC_LIB_VERSION})
_install(linuxcncshm)

install(FILES shmdrv.rules
    DESTINATION /etc/udev/rules.d
    RENAME 50-shmdrv.rules)

install(FILES limits.d-machinekit.conf
    DESTINATION /etc/security/limits.d
    RENAME machinekit.conf)
