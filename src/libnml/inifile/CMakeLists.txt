add_library(inifile OBJECT inifile.cc)
target_compile_options(inifile PUBLIC -fPIC)

add_library(linuxcncini SHARED $<TARGET_OBJECTS:inifile>)
set_target_properties(linuxcncini PROPERTIES VERSION ${GENERIC_LIB_VERSION})
_install(linuxcncini)


add_executable(inivar inivar.cc)
set_target_properties(inivar PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_LIBEXEC_DIR})
target_link_libraries(inivar linuxcncini)
_install_exec(inivar)
