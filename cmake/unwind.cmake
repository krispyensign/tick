include(ExternalProject)
set(env{_XOPEN_SOURCE} on)
ExternalProject_Add(
  unwind
  PREFIX ${CMAKE_BINARY_DIR}/unwind
  GIT_REPOSITORY "https://github.com/libunwind/libunwind"
  GIT_TAG master
  GIT_PROGRESS true
  CONFIGURE_COMMAND ./autogen.sh COMMAND "./configure"
  INSTALL_COMMAND ""
  TEST_COMMAND ""
  BUILD_IN_SOURCE 1
  BUILD_COMMAND "make"
)

ExternalProject_Get_Property(unwind SOURCE_DIR)
ExternalProject_Get_Property(unwind BINARY_DIR)
set(UNWIND_INCLUDE_DIR ${SOURCE_DIR}/include CACHE INTERNAL "Path to include unwind")
set(UNWIND_LIBRARY_DIR ${BINARY_DIR} CACHE INTERNAL CACHE INTERNAL "Path to library unwind")

# avoid errors defining targets twice
if (TARGET libunwind)
    return()
endif()

add_library(libunwind SHARED IMPORTED)
add_dependencies(libunwind unwind)

if(APPLE)
set(SUFFIX "dylib")
elseif(WIN32)
set(SUFFIX "dll")
else()
set(SUFFIX "so")
endif()
set_property(TARGET libunwind PROPERTY IMPORTED_LOCATION "${UNWIND_LIBRARY_DIR}/libunwind.${SUFFIX}")
