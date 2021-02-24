cmake_minimum_required(VERSION 3.10)
project(tick VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CONAN_DISABLE_CHECK_COMPILER 1)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(MSGPACK_CXX17 ON)
set(FMT_HEADER_ONLY ON)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_compile_options(
  -fno-omit-frame-pointer
  -fsanitize=address
  -Wall
  -g
  -Wextra
  -pedantic
)

add_link_options(
  -fno-omit-frame-pointer
  -fsanitize=address
  -Wall
  -g
  -Wextra
  -pedantic
)

if(CMAKE_CXX_CPPCHECK)
else()
set(CMAKE_CXX_CPPCHECK "cppcheck")
endif(CMAKE_CXX_CPPCHECK)

list(APPEND CMAKE_CXX_CPPCHECK
        "--suppress=*:$ENV{HOME}/.conan/data/boost/*"
        "--suppress=*:$ENV{HOME}/.conan/data/cpprestsdk/*"
        "--enable=all"
        "--inconclusive"
        "--check-config"
        "--suppress=missingIncludeSystem"
        "--report-progress"
)

find_package(OpenSSL REQUIRED COMPONENTS SSL Crypto)

include(cmake/FindZeroMQ.cmake)
include(cmake/cppzmq.cmake)
include(cmake/backwardcpp.cmake)
include(cmake/rapidjson.cmake)
include(cmake/fmtlib.cmake)
include(cmake/spdlog.cmake)
include(cmake/msgpack.cmake)

externalproject_add_stepdependencies(spdlog build fmtlib)

add_executable(tick
    src/main.cpp
    src/ticker_service.cpp
    src/kraken.cpp
)

add_dependencies(tick
  rapidjson
  backward-cpp
  cppzmq
  fmtlib
  spdlog
  msgpack
)

target_include_directories(tick
  PUBLIC
  ${BACKWARD_CPP_INCLUDE_DIR}
  ${RAPIDJSON_INCLUDE_DIR}
  ${PC_LIBZMQ_INCLUDE_DIRS}
  ${CPPZMQ_INCLUDE_DIR}
  ${OPENSSL_INCLUDE_DIR}
  ${FMTLIB_INCLUDE_DIR}
  ${SPDLOG_INCLUDE_DIR}
  ${MSGPACK_INCLUDE_DIR}
)

target_link_libraries(tick
    ${CONAN_LIBS}
    OpenSSL::Crypto
    OpenSSL::SSL
    libzmq
)
