#define BACKWARD_HAS_LIBUNWIND 1
#include "backward.hpp"
#include "ticker_service.hpp"
// TODO: add main argument parser and options file
// TODO: add sigint handler

backward::SignalHandling sh;

auto main(i16 argc, c_str argv[]) -> i16 {
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
