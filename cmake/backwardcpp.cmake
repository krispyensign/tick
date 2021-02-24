include(ExternalProject)

ExternalProject_Add(
  backward-cpp
  PREFIX ${CMAKE_BINARY_DIR}/backward-cpp
  GIT_REPOSITORY "https://github.com/bombela/backward-cpp"
  GIT_TAG master
  INSTALL_COMMAND ""
  TEST_COMMAND ""
  BUILD_COMMAND ""
  CMAKE_ARGS "-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}"
)

ExternalProject_Get_Property(backward-cpp SOURCE_DIR)
set(BACKWARD_CPP_INCLUDE_DIR ${SOURCE_DIR}/ CACHE INTERNAL "Path to include backward-cpp")
