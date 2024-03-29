cmake_minimum_required(VERSION 3.10)
project(tick VERSION 1.0)

################################################################################
## Settings
################################################################################
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(MSGPACK_CXX17 ON)
set(FMT_HEADER_ONLY ON)
# set(CMAKE_CXX_CPPCHECK cppcheck --enable=all --inconclusive --suppress=preprocessorErrorDirective --suppress=missingIncludeSystem --suppress=unmatchedSuppression --report-progress)
# set(CMAKE_CXX_CLANG_TIDY clang-tidy -checks=-*,,clang-analyzer-*,-clang-analyzer-cplusplus*,bugprone-*,readability-*,concurrency-*,modernize-*,performance-*,portability-*)
set(cpprestsdk_DIR /usr/lib/${CMAKE_LIBRARY_ARCHITECTURE}/cmake/)

################################################################################
## Finders / Packages
################################################################################
include(cmake/findzmq.cmake)
if(NOT APPLE)
  include(cmake/findunwind.cmake)
endif()

find_library(CPPREST_LIB cpprest)
find_package(cpprestsdk REQUIRED NAMES cpprestsdk cpprest)
find_package(OpenSSL REQUIRED COMPONENTS SSL Crypto)
find_package(Boost REQUIRED COMPONENTS random system thread filesystem chrono atomic date_time regex)

################################################################################
## Compile
################################################################################
add_compile_options(-fno-omit-frame-pointer -fsanitize=address -Wall -g -Wextra -pedantic)
add_link_options(-fno-omit-frame-pointer -fsanitize=address -Wall -g -Wextra -pedantic)
file(GLOB TICK_SOURCE_FILES
  "src/common/*.hpp"
  "src/common/*.cpp"

  "src/deps/*.hpp"
  "src/deps/*.cpp"

  "src/tick/*.cpp"
  "src/tick/*.hpp"
)
add_executable(tick ${TICK_SOURCE_FILES})
target_include_directories(tick PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/third_party/ ${PC_LIBZMQ_INCLUDE_DIRS} ${OPENSSL_INCLUDE_DIR} ${BOOST_INCLUDE_DIR} ${CPPRESTSDK_INCLUDE_DIR})

################################################################################
## Link
################################################################################
if(APPLE)
  target_link_libraries(tick "-framework System")
else()
  target_link_libraries(tick libunwind)
endif()
target_link_libraries(tick Boost::boost Boost::random Boost::system Boost::thread Boost::filesystem
  Boost::chrono Boost::atomic Boost::date_time Boost::regex OpenSSL::Crypto OpenSSL::SSL
  ${CPPREST_LIB} libzmq)
