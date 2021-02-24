include(ExternalProject)

ExternalProject_Add(
  websocketpp
  PREFIX ${CMAKE_BINARY_DIR}/websocketpp
  GIT_REPOSITORY "https://github.com/zaphoyd/websocketpp"
  GIT_TAG master
  GIT_PROGRESS true
  BUILD_COMMAND ""
  INSTALL_COMMAND ""
  TEST_COMMAND ""
  CMAKE_ARGS "-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}"
)

ExternalProject_Get_Property(websocketpp SOURCE_DIR)
set(WEBSOCKETPP_INCLUDE_DIR ${SOURCE_DIR}/ CACHE INTERNAL "Path to include websocketpp")
