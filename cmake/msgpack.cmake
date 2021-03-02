include(ExternalProject)

ExternalProject_Add(
  msgpack
  PREFIX ${CMAKE_BINARY_DIR}/msgpack
  GIT_REPOSITORY "https://github.com/msgpack/msgpack-c.git"
  GIT_TAG cpp_master
  GIT_PROGRESS true
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  TEST_COMMAND ""
  INSTALL_COMMAND ""
  CMAKE_ARGS "-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}"
)

ExternalProject_Get_Property(msgpack SOURCE_DIR)
set(MSGPACK_INCLUDE_DIR ${SOURCE_DIR}/include/ CACHE INTERNAL "Path to include msgpack")
