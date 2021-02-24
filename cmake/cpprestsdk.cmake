include(ExternalProject)

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
set(CPPRESTSDK_INCLUDE_DIR ${SOURCE_DIR}/Release/include/ CACHE INTERNAL "Path to include cpprestsdk")
set(CPPRESTSDK_LIBRARY_DIR ${BINARY_DIR}/Release/Binaries/ CACHE INTERNAL "Path to library cpprestsdk")

find_library(cpprestsdk_LIBRARY NAMES libcpprest.so libcpprest.dylib libcpprest.dll
             PATHS ${CPPRESTSDK_LIBRARY_DIR})

if(cpprestsdk_LIBRARY)
    set(cpprestsdk_FOUND ON)
endif()

if (TARGET libcpprest)
    # avoid errors defining targets twice
    return()
endif()

add_library(libcpprest SHARED IMPORTED)
set_property(TARGET libcpprest PROPERTY IMPORTED_LOCATION ${cpprestsdk_LIBRARY})
