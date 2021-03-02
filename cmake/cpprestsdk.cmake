include(ExternalProject)

set(BUILD_TESTS CACHE INTERNAL off)
set(BUILD_SAMPLES CACHE INTERNAL off)

ExternalProject_Add(
  cpprestsdk
  PREFIX ${CMAKE_BINARY_DIR}/cpprestsdk
  GIT_REPOSITORY "https://github.com/microsoft/cpprestsdk"
  GIT_TAG master
  GIT_PROGRESS true
  INSTALL_COMMAND ""
  TEST_COMMAND ""
  CMAKE_ARGS "-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}" "-DBUILD_TESTS=off" "-DBUILD_SAMPLES=off"
)

ExternalProject_Get_Property(cpprestsdk SOURCE_DIR)
ExternalProject_Get_Property(cpprestsdk BINARY_DIR)
set(CPPRESTSDK_INCLUDE_DIR ${SOURCE_DIR}/Release/include CACHE INTERNAL "Path to include cpprestsdk")
set(CPPRESTSDK_LIBRARY_DIR ${BINARY_DIR}/Release/Binaries CACHE INTERNAL "Path to library cpprestsdk")

# avoid errors defining targets twice
if (TARGET libcpprest)
    return()
endif()

add_library(libcpprest SHARED IMPORTED)
add_dependencies(libcpprest cpprestsdk)

if(APPLE)
set(SUFFIX "dylib")
elseif(WIN32)
set(SUFFIX "dll")
else()
set(SUFFIX "so")
endif()
set_property(TARGET libcpprest PROPERTY IMPORTED_LOCATION "${CPPRESTSDK_LIBRARY_DIR}/libcpprest.${SUFFIX}")
