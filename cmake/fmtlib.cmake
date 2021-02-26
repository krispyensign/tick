include(ExternalProject)

ExternalProject_Add(
  fmtlib_git
  PREFIX ${CMAKE_BINARY_DIR}/fmtlib
  GIT_REPOSITORY "https://github.com/fmtlib/fmt/"
  GIT_TAG master
  GIT_PROGRESS true
  TEST_COMMAND ""
  INSTALL_COMMAND ""
  CMAKE_ARGS "-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}" "-DFMT_TEST=off" "-DFMT_INSTALL=off"
)

ExternalProject_Get_Property(fmtlib_git SOURCE_DIR)
ExternalProject_Get_Property(fmtlib_git BINARY_DIR)
set(FMTLIB_INCLUDE_DIR ${SOURCE_DIR}/include/ CACHE INTERNAL "Path to include fmtlib")
set(FMTLIB_LIBRARY_DIR ${BINARY_DIR}/ CACHE INTERNAL "Path to library fmtlib")

# avoid errors defining targets twic
if (TARGET fmtlib)
    return()
endif()

add_library(fmtlib STATIC IMPORTED)
add_dependencies(fmtlib fmtlib_git)

if(APPLE)
set(SUFFIX "a")
elseif(WIN32)
set(SUFFIX "dll")
else()
set(SUFFIX "a")
endif()

set_property(TARGET fmtlib PROPERTY IMPORTED_LOCATION "${FMTLIB_LIBRARY_DIR}/libfmt.${SUFFIX}")
