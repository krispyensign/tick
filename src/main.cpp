#define BACKWARD_HAS_LIBUNWIND 1
#include "backward.hpp"
#include "ticker_service.hpp"
// TODO: add main argument parser and options file

backward::SignalHandling sh;
function<void(void)> shutdown_func;

auto sigint_handler(int) -> void{
  shutdown_func();
}

auto main(i16 argc, c_str argv[]) -> i16 {

  signal(SIGINT, sigint_handler);

  try {
    auto conf = service_config{
      .zbind = "tcp://*:9000",
      .ws_uri = "wss://ws.kraken.com",
      .api_url = "https://api.kraken.com",
      .assets_path = "/0/public/AssetPairs",
    };

    auto [shutdown_func, service] = ticker_service::tick_service(exchange_name::KRAKEN, conf);

  } catch (const exception& e) {
    logger::error(e.what());
    throw e;
  }
  return 0;
}
