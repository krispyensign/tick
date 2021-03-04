set(PKG_CONFIG_USE_CMAKE_PREFIX_PATH ON)

find_package(PkgConfig)
pkg_check_modules(PC_LIBUNWIND QUIET libunwind)

set(Unwind_VERSION ${PC_LIBUNWIND_VERSION})
find_library(Unwind_LIBRARY NAMES libunwind.so libunwind.dylib libunwind.dll
             PATHS ${PC_LIBUNWIND_LIBDIR} ${PC_LIBUNWIND_LIBRARY_DIRS})

if(Unwind_LIBRARY)
    set(Unwind_FOUND ON)
endif()

if (TARGET libunwind)
    # avoid errors defining targets twice
    return()
endif()

add_library(libunwind SHARED IMPORTED)
set_property(TARGET libunwind PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${PC_LIBUNWIND_INCLUDE_DIRS})
set_property(TARGET libunwind PROPERTY IMPORTED_LOCATION ${Unwind_LIBRARY})
