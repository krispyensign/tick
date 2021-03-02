find_package(PkgConfig)
pkg_check_modules(PC_LIBUNWIND QUIET libunwind)

set(LIBUNWIND_VERSION ${PC_LIBUNWIND_VERSION})
find_library(LIBUNWIND_LIBRARY NAMES libunwind.so libunwind.dylib libunwind.dll
             PATHS ${PC_LIBUNWIND_LIBDIR} ${PC_LIBUNWIND_LIBRARY_DIRS})

if(LIBUNWIND_LIBRARY)
    set(LIBUNWIND_FOUND ON)
endif()

if (TARGET libunwind)
    # avoid errors defining targets twice
    return()
endif()

add_library(libunwind SHARED IMPORTED)
set_property(TARGET libunwind PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${PC_LIBUNWIND_INCLUDE_DIRS})
