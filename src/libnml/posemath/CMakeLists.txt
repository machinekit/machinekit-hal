add_library(posemath SHARED _posemath.c posemath.cc gomath.c sincos.c)
target_link_libraries(posemath rtapi_math)
set_target_properties(posemath PROPERTIES VERSION ${GENERIC_LIB_VERSION})
_install(posemath)
