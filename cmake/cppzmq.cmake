include(ExternalProject)

ExternalProject_Add(
  cppzmq
  PREFIX ${CMAKE_BINARY_DIR}/cppzmq
  GIT_REPOSITORY "https://github.com/zeromq/cppzmq"
  GIT_TAG master
  GIT_PROGRESS true
  BUILD_COMMAND ""
  TEST_COMMAND ""
  INSTALL_COMMAND ""
  CMAKE_ARGS "-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}"
)

ExternalProject_Get_Property(cppzmq SOURCE_DIR)
set(CPPZMQ_INCLUDE_DIR ${SOURCE_DIR} CACHE INTERNAL "Path to include cppzmq")
set(PKG_CONFIG_USE_CMAKE_PREFIX_PATH ON)

find_package(PkgConfig)
pkg_check_modules(PC_LIBZMQ QUIET libzmq)

set(ZeroMQ_VERSION ${PC_LIBZMQ_VERSION})
find_library(ZeroMQ_LIBRARY NAMES libzmq.so libzmq.dylib libzmq.dll
             PATHS ${PC_LIBZMQ_LIBDIR} ${PC_LIBZMQ_LIBRARY_DIRS})
find_library(ZeroMQ_STATIC_LIBRARY NAMES libzmq-static.a libzmq.a libzmq.dll.a
             PATHS ${PC_LIBZMQ_LIBDIR} ${PC_LIBZMQ_LIBRARY_DIRS})

if(ZeroMQ_LIBRARY OR ZeroMQ_STATIC_LIBRARY)
    set(ZeroMQ_FOUND ON)
endif()

if (TARGET libzmq)
    # avoid errors defining targets twice
    return()
endif()

add_library(libzmq SHARED IMPORTED)
set_property(TARGET libzmq PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${PC_LIBZMQ_INCLUDE_DIRS})
set_property(TARGET libzmq PROPERTY IMPORTED_LOCATION ${ZeroMQ_LIBRARY})

add_library(libzmq-static STATIC IMPORTED ${PC_LIBZMQ_INCLUDE_DIRS})
set_property(TARGET libzmq-static PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${PC_LIBZMQ_INCLUDE_DIRS})
set_property(TARGET libzmq-static PROPERTY IMPORTED_LOCATION ${ZeroMQ_STATIC_LIBRARY})
