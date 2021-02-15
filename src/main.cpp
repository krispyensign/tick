#include "ticker_service.hpp"


def main(i16 argc, c_str argv[]) -> i16 {
  let ms = make_shared<micro_service>();
  return tick_service(ms);
}
