include(ExternalProject)

ExternalProject_Add(
  backwardcpp
  PREFIX ${CMAKE_BINARY_DIR}/backwardcpp
  GIT_REPOSITORY "https://github.com/bombela/backward-cpp"
  GIT_TAG master
  GIT_PROGRESS true
  INSTALL_COMMAND ""
  CMAKE_ARGS "-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}"
)

ExternalProject_Get_Property(backwardcpp SOURCE_DIR)
set(BACKWARDCPP_INCLUDE_DIR ${SOURCE_DIR}/ CACHE INTERNAL "Path to backwardcpp includes")

