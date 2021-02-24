cmake_minimum_required(VERSION 3.10)
project(tick VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CONAN_DISABLE_CHECK_COMPILER 1)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(MSGPACK_CXX17 ON)
set(FMT_HEADER_ONLY ON)

if(CMAKE_CXX_CPPCHECK)
else()
set(CMAKE_CXX_CPPCHECK "cppcheck")
endif(CMAKE_CXX_CPPCHECK)

list(APPEND CMAKE_CXX_CPPCHECK "--suppress=*:$ENV{HOME}/.conan/data/boost/*"
  "--suppress=*:$ENV{HOME}/.conan/data/cpprestsdk/*" "--enable=all" "--inconclusive"
  "--check-config" "--suppress=missingIncludeSystem" "--report-progress"
)

find_package(OpenSSL REQUIRED COMPONENTS SSL Crypto)
find_package(Boost REQUIRED COMPONENTS iostreams random system thread filesystem chrono
             atomic date_time regex unit_test_framework log log_setup)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

include(cmake/FindZeroMQ.cmake)
include(cmake/cppzmq.cmake)
include(cmake/backwardcpp.cmake)
include(cmake/rapidjson.cmake)
include(cmake/fmtlib.cmake)
include(cmake/spdlog.cmake)
include(cmake/msgpack.cmake)
externalproject_add_stepdependencies(spdlog build fmtlib)

file(GLOB TICK_SOURCE_FILES "src/*.cpp" "src/*.hpp")
add_compile_options(-fno-omit-frame-pointer -fsanitize=address -Wall -g -Wextra -pedantic)
add_link_options(-fno-omit-frame-pointer -fsanitize=address -Wall -g -Wextra -pedantic)
add_executable(tick ${TICK_SOURCE_FILES})
add_dependencies(tick rapidjson backward-cpp cppzmq fmtlib spdlog msgpack)

target_include_directories(tick
  PUBLIC
  ${BACKWARD_CPP_INCLUDE_DIR}
  ${RAPIDJSON_INCLUDE_DIR}
  ${PC_LIBZMQ_INCLUDE_DIRS}
  ${CPPZMQ_INCLUDE_DIR}
  ${OPENSSL_INCLUDE_DIR}
  ${BOOST_INCLUDE_DIR}
  ${FMTLIB_INCLUDE_DIR}
  ${SPDLOG_INCLUDE_DIR}
  ${MSGPACK_INCLUDE_DIR}
)

target_link_libraries(tick
  Boost::boost Boost::random Boost::system Boost::thread Boost::filesystem Boost::chrono
  Boost::atomic Boost::date_time Boost::regex Boost::log
  OpenSSL::Crypto OpenSSL::SSL
  ${CONAN_LIBS}
  libzmq
)
