#define BACKWARD_HAS_LIBUNWIND 1
#include <spdlog/spdlog.h>

#include <args.hxx>
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
  let table = unordered_map<str, exchange_name>{{"kraken", exchange_name::kraken}};
  // setup the parser
  args::ArgumentParser parser("Websocket and ZeroMQ tick replicator");
  let help = args::HelpFlag(parser, "help", "Display this help menu", {'h', "help"});
  auto zbind
    = args::ValueFlag<str>(parser, "zbind", "Publisher queue Uri", {'z', "zbind"}, "tcp://*:9000");
  auto name
    = args::ValueFlag<str>(parser, "exchange_name", "Exchange name", {'e', "exchange"}, "kraken");

  exchange_name exident;
  if (let it = table.find(name.Get()); it != table.end()) {
    exident = it->second;
  } else {
    cout << "Unsupported exchange: " << name << endl;
    std::cout << parser;
    return 0;
  }

  // parse the cli args
  try {
    parser.ParseCLI(argc, argv);
  } catch (args::Help&) {
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
    logger::info("using exchange: {}", name.Get());
    logger::info("using publisher binding: {}", zbind.Get());
    async(launch::async, ticker_service::tick_service, exident, zbind.Get(),
          cref(cancellation_token))
      .get();
  } catch (const exception& e) {
    logger::error(e.what());
    throw;
  }
  return 0;
}
