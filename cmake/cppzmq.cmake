include(ExternalProject)

ExternalProject_Add(
  cppzmq
  PREFIX ${CMAKE_BINARY_DIR}/cppzmq
  GIT_REPOSITORY "https://github.com/zeromq/cppzmq"
  GIT_TAG master
  BUILD_COMMAND ""
  INSTALL_COMMAND ""
  TEST_COMMAND ""
  CMAKE_ARGS "-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}"
)

ExternalProject_Get_Property(cppzmq SOURCE_DIR)
set(CPPZMQ_INCLUDE_DIR ${SOURCE_DIR} CACHE INTERNAL "Path to include cppzmq")
