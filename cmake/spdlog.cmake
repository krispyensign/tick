include(ExternalProject)

ExternalProject_Add(
  spdlog
  PREFIX ${CMAKE_BINARY_DIR}/spdlog
  GIT_REPOSITORY "https://github.com/gabime/spdlog"
  GIT_TAG v1.x
  GIT_PROGRESS true
  BUILD_COMMAND ""
  TEST_COMMAND ""
  INSTALL_COMMAND ""
  CMAKE_ARGS "-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}"
)

ExternalProject_Get_Property(spdlog SOURCE_DIR)
set(SPDLOG_INCLUDE_DIR ${SOURCE_DIR}/include/ CACHE INTERNAL "Path to include spdlog")
