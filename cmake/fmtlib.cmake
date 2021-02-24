include(ExternalProject)

ExternalProject_Add(
  fmtlib
  PREFIX ${CMAKE_BINARY_DIR}/fmtlib
  GIT_REPOSITORY "https://github.com/fmtlib/fmt/"
  GIT_TAG master
  BUILD_COMMAND ""
  INSTALL_COMMAND ""
  TEST_COMMAND ""
  CMAKE_ARGS "-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}"
)

ExternalProject_Get_Property(fmtlib SOURCE_DIR)
set(FMTLIB_INCLUDE_DIR ${SOURCE_DIR}/include/ CACHE INTERNAL "Path to include fmtpack")
