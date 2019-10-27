set(COMP_SRC remote ringread ringwrite)

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

set(_src pbmsgs.c $<TARGET_OBJECTS:npb-obj>)
_to_rtlib(pbmsgs ${_src})
add_dependencies(pbmsgs npb_gen_files)


unset(_gen_man)
# create manpages
foreach(file ${COMP_SRC})
    _comp_to_man(${file})
endforeach()
add_custom_target(man_msgcomponents ALL DEPENDS ${_gen_man})
_install_man("${_gen_man}")
