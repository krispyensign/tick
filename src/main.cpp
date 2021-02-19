#include "ticker_service.hpp"

// TODO: add main argument parser
// TODO: add sigint handler
// TODO: add logger
// TODO: add stacktracing

def main(i16 argc, c_str argv[]) -> i16 {
  let ms = make_shared<micro_service>();
  return tick_service(ms);
}
