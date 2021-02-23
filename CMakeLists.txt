cmake_minimum_required(VERSION 3.10)
project(tick VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CONAN_DISABLE_CHECK_COMPILER 1)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

list(
    APPEND CMAKE_CXX_FLAGS_DEBUG
        "-fno-omit-frame-pointer"
        "-fsanitize=all"
        "-fanalyzer"
        "-Werror"
        "-Wall"
        # "-DBOOST_STACKTRACE_USE_ADDR2LINE"
        "-g"
)

list(
    APPEND CMAKE_LINKER_FLAGS_DEBUG
        "-fno-omit-frame-pointer"
        "-fsanitize=all"
        "-fanalyzer"
        "-Werror"
        "-Wall"
        # "-DBOOST_STACKTRACE_USE_ADDR2LINE"
        "-g"
)

# if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
# list(
#     APPEND CMAKE_CXX_FLAGS_DEBUG
#     "-D_GNU_SOURCE"
# )
# list(
#     APPEND CMAKE_LINKER_FLAGS_DEBUG
#     "-D_GNU_SOURCE"
# )
# else()
# list(
#     APPEND CMAKE_CXX_FLAGS_DEBUG
#     "-DBOOST_STACKTRACE_GNU_SOURCE_NOT_REQUIRED"
# )
# list(
#     APPEND CMAKE_LINKER_FLAGS_DEBUG
#     "-DBOOST_STACKTRACE_GNU_SOURCE_NOT_REQUIRED"
# )
# endif()

if(CMAKE_CXX_CPPCHECK)
else()
set(CMAKE_CXX_CPPCHECK "cppcheck")
endif(CMAKE_CXX_CPPCHECK)

list(
    APPEND CMAKE_CXX_CPPCHECK
        "--suppress=*:$ENV{HOME}/.conan/data/boost/*"
        "--suppress=*:$ENV{HOME}/.conan/data/cpprestsdk/*"
        "--enable=all"
        "--inconclusive"
        "--check-config"
        "--suppress=missingIncludeSystem"
        "--report-progress"
)

add_executable(tick src/main.cpp src/ticker_service.cpp src/kraken.cpp)
target_link_libraries(tick ${CONAN_LIBS})
