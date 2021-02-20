#include "ticker_service.hpp"
#include "types.hpp"

// TODO: add main argument parser
// TODO: add sigint handler
// TODO: add stacktracing

def main(i16 argc, c_str argv[]) -> i16 {
  let conf = config {
    .zbind = "tcp://*:9000",
    .ws_uri = "wss://ws.kraken.com",
  };
  tick_service(conf);
  return 0;
}
