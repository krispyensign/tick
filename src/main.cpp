#define BACKWARD_HAS_LIBUNWIND 1
#include "backward.hpp"
#include "ticker_service.hpp"
// TODO: add main argument parser and options file

backward::SignalHandling sh;

namespace {
std::function<void(int)> shutdown_handler;
void signal_handler(int signal) { shutdown_handler(signal); }
}

auto main(i16 argc, c_str argv[]) -> i16 {

  // setup the cancellation token for the service to catch console ctrl-c
  auto cancellation_token = atomic_bool(true);
  shutdown_handler = [&cancellation_token](int) { cancellation_token = false; };
  signal(SIGINT, signal_handler);

  try {
    // setup the config
    auto conf = service_config{
      .zbind = "tcp://*:9000",
      .ws_uri = "wss://ws.kraken.com",
      .api_url = "https://api.kraken.com",
      .assets_path = "/0/public/AssetPairs",
    };

    // launch the service
    async(launch::async, ticker_service::tick_service, exchange_name::KRAKEN, conf,
                         ref(cancellation_token)).get();

  } catch (const exception& e) {
    logger::error(e.what());
    throw e;
  }
  return 0;
}
