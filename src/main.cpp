#include "args.hxx"
#define BACKWARD_HAS_LIBUNWIND 1
#include <spdlog/spdlog.h>

#include <backward.hpp>
#include <future>

#include "ticker_service.hpp"
#include "types.hpp"

using namespace std;
// setup stacktracing
backward::SignalHandling sh;

// handlers for graceful shutdown on ctrl-c
namespace {
function<void(int)> shutdown_handler;
auto signal_handler(int signal) -> void { shutdown_handler(signal); }
}  // namespace

auto main(i16 argc, c_str argv[]) -> i16 {
  // setup the parser
  args::ArgumentParser parser("Websocket and ZeroMQ tick replicator");
  auto cli = cli_config{
    .help = args::HelpFlag(parser, "help", "Display this help menu", {'h', "help"}),
    .zbind = args::ValueFlag<str>(parser, "zbind", "TCP URI i.e. tcp://*:9000", {'z', "zbind"},
                                  "tcp://*:9000"),
    .exchange = args::ValueFlag<exchange_name>(parser, "exchange_name", "Exchange name i.e. kraken",
                                               {'e', "exchange"}, exchange_name::kraken)};

  // parse the cli args
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

  // launch the service
  try {
    async(launch::async, ticker_service::tick_service, cli.exchange.Get(), cli.zbind.Get(),
          ref(cancellation_token))
      .get();
  } catch (const exception& e) {
    logger::error(e.what());
    throw;
  }
  return 0;
}
