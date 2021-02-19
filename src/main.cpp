#include "ticker_service.hpp"
#include "types.hpp"

// TODO: add main argument parser
// TODO: add sigint handler
// TODO: add logger
// TODO: add stacktracing

def main(i16 argc, c_str argv[]) -> i16 {
  let conf = config {
    .zbind = "tcp://localhost:5000",
    .ws_uri = "ws://kraken.api",
  };
  tick_service(conf);
  return 0;
}
