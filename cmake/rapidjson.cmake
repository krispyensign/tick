include(ExternalProject)

ExternalProject_Add(
  rapidjson
  PREFIX ${CMAKE_BINARY_DIR}/rapidjson
  GIT_REPOSITORY "https://github.com/Tencent/rapidjson"
  GIT_TAG master
  GIT_PROGRESS true
  TEST_COMMAND ""
  INSTALL_COMMAND ""
  CMAKE_ARGS "-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}" "-DRAPIDJSON_BUILD_TESTS=off"
)

ExternalProject_Get_Property(rapidjson SOURCE_DIR)
set(RAPIDJSON_INCLUDE_DIR ${SOURCE_DIR}/include/ CACHE INTERNAL "Path to include rapidjson")
