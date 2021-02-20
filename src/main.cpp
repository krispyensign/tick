#include "ticker_service.hpp"
#include "types.hpp"
#include <spdlog/spdlog.h>

// TODO: add main argument parser
// TODO: add sigint handler
// TODO: add stacktracing

def main(i16 argc, c_str argv[]) -> i16 {
  let conf = config {
    .zbind = "tcp://*:9000",
    .ws_uri = "wss://ws.kraken.com",
  };
  try {
    tick_service(conf);
  } catch (const exception& e) {
    spdlog::error(e.what());
    throw e;
  }
  return 0;
}
