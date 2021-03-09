#define BACKWARD_HAS_LIBUNWIND 1
#include <spdlog/spdlog.h>

#include <backward.hpp>
#include <future>

#include "ticker_service.hpp"
#include "types.hpp"

using namespace std;
// TODO: add main argument parser and options file

// setup stacktracing
backward::SignalHandling sh;

// handlers for graceful shutdown on ctrl-c
namespace {
function<void(int)> shutdown_handler;
auto signal_handler(int signal) -> void { shutdown_handler(signal); }
}  // namespace

auto main(i16 argc, c_str argv[]) -> i16 {
  args::ArgumentParser parser("Websocket and ZeroMQ tick replicator");
  auto cli = cli_config{
    .help = args::HelpFlag(parser, "help", "Display this help menu", {'h', "help"}),
    .zbind = args::ValueFlag<str>(parser, "zbind", "TCP URI i.e. tcp://*:9000", {'z', "zbind"}),
    .ws_uri = args::ValueFlag<str>(parser, "ws_uri", "WebSocket URI i.e. wss://ws.kraken.com",
                                   {'w', "ws_uri"}),
    .api_url = args::ValueFlag<str>(parser, "api_url", "API URL i.e https://api.kraken.com",
                                    {'a', "api_url"}),
    .asset_path = args::ValueFlag<str>(
      parser, "assets_path", "URL path to look up exchange assets i.e. /0/public/AssetsPairs",
      {'p', "asset_paths"})};

  try {
    parser.ParseCLI(argc, argv);
  } catch (args::Help) {
    std::cout << parser;
    return 0;
  }
  // setup the cancellation token for the service to catch console ctrl-c
  auto cancellation_token = atomic_bool(true);
  shutdown_handler = [&cancellation_token](int a) -> void {
    logger::info(fmt::format("Got signal: {}", a));
    cancellation_token = false;
  };
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
    async(launch::async, ticker_service::tick_service, exchange_name::kraken, conf,
          ref(cancellation_token))
      .get();

  } catch (const exception& e) {
    logger::error(e.what());
    throw;
  }
  return 0;
}
