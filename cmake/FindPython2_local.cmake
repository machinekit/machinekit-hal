# Replace some variables provided by FindPython2.cmake for cmake
# versions before 3.12
#
# https://cmake.org/cmake/help/v3.12/module/FindPython2.html
#
# Variables:
# Python2_FOUND
# Python2_SITELIB
# Python2_EXECUTABLE
# Python2_VERSION
# Python2_INCLUDE_DIRS

find_package(PythonInterp ${Python2_local_FIND_VERSION} REQUIRED)
find_package(PythonLibs ${Python2_local_FIND_VERSION} REQUIRED)

if (PYTHON_VERSION_STRING AND PYTHONLIBS_VERSION_STRING)
  if(NOT PYTHON_VERSION_STRING VERSION_EQUAL PYTHONLIBS_VERSION_STRING)
    message(FATAL_ERROR
      "Version mismatch between python interpreter and libraries")
  endif()
endif()

set (Python2_FOUND TRUE)
set (Python2_EXECUTABLE ${PYTHON_EXECUTABLE})
set (Python2_VERSION ${PYTHON_VERSION_STRING})

execute_process (
  COMMAND python -c "from distutils.sysconfig import get_python_lib; print get_python_lib()"
  OUTPUT_VARIABLE Python2_SITELIB  OUTPUT_STRIP_TRAILING_WHITESPACE)

execute_process (
  COMMAND python -c "from distutils.sysconfig import get_python_inc; print get_python_inc()"
  OUTPUT_VARIABLE Python2_INCLUDE_DIRS  OUTPUT_STRIP_TRAILING_WHITESPACE)
