cmake_minimum_required(VERSION 3.10)
project(tick VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(MSGPACK_CXX17 ON)
set(FMT_HEADER_ONLY ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

include(cmake/findzmq.cmake)
include_directories(third_party)

set(cpprestsdk_DIR /usr/lib/${CMAKE_LIBRARY_ARCHITECTURE}/cmake/)
find_library(CPPREST_LIB cpprest)
find_package(cpprestsdk REQUIRED NAMES cpprestsdk cpprest)
find_package(OpenSSL REQUIRED COMPONENTS SSL Crypto)

find_package(Boost REQUIRED COMPONENTS random system thread filesystem chrono atomic
  date_time regex)

set(CMAKE_CXX_CPPCHECK "cppcheck"
  "--enable=all"
  "--inconclusive"
  "--suppress=preprocessorErrorDirective"
  "--suppress=missingIncludeSystem"
  "--suppress=unmatchedSuppression"
  "--report-progress"
)

set(CMAKE_CXX_CLANG_TIDY clang-tidy -checks=-*,readability-*)

add_compile_options(-fno-omit-frame-pointer -fsanitize=address -Wall -g -Wextra -pedantic)
add_link_options(-fno-omit-frame-pointer -fsanitize=address -Wall -g -Wextra -pedantic)
file(GLOB TICK_SOURCE_FILES "src/*.cpp" "src/*.hpp")
add_executable(tick ${TICK_SOURCE_FILES})

target_include_directories(tick PUBLIC
  ${PC_LIBZMQ_INCLUDE_DIRS}
  ${OPENSSL_INCLUDE_DIR}
  ${BOOST_INCLUDE_DIR}
  ${CPPRESTSDK_INCLUDE_DIR}
)

if(APPLE)
  target_link_libraries(tick "-framework System")
else()
  include(cmake/findunwind.cmake)
  target_link_libraries(tick libunwind)
endif()

target_link_libraries(tick
  Boost::boost
  Boost::random
  Boost::system
  Boost::thread
  Boost::filesystem
  Boost::chrono
  Boost::atomic
  Boost::date_time
  Boost::regex
  OpenSSL::Crypto
  OpenSSL::SSL
  ${CPPREST_LIB}
  libzmq
)
