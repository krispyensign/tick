#line __LINE__ "system.cpp"

#include <execinfo.h>
#include <getopt.h>
#include <sys/wait.h>
#include <unistd.h>

#include "ticker_service.hpp"

// TODO: add main argument parser and options file
// TODO: add sigint handler

#ifndef BACKTRACE_LENGTH
#define BACKTRACE_LENGTH 20
#endif

auto backtrace_handler(int sig) -> void {
  auto backtrace_array = array<void *, BACKTRACE_LENGTH>();
  // get void*'s for all entries on the stack
  auto size = backtrace(backtrace_array.data(), BACKTRACE_LENGTH);

  // print out all the frames to stderr
  std::cerr << "Error: signal " << sig << std::endl;
  backtrace_symbols_fd(backtrace_array.data(), size, STDERR_FILENO);

  // exit with an error code
  exit(1);
}

auto main(i16 argc, c_str argv[]) -> i16 {
  signal(SIGSEGV, backtrace_handler);
  signal(SIGABRT, backtrace_handler);

  try {
    auto conf = service_config{
      .zbind = "tcp://*:9000",
      .ws_uri = "wss://ws.kraken.com",
      .api_url = "https://api.kraken.com",
      .assets_path = "/0/public/AssetPairs",
    };
    ticker_service::tick_service(exchange_name::KRAKEN, conf);
  } catch (const exception& e) {
    logger::error(e.what());
    throw e;
  }
  return 0;
}
