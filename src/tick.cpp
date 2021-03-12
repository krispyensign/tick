#include <functional>
#include "ticker_service.hpp"

using std::cout, std::launch, args::HelpFlag, args::ArgumentParser, args::ValueFlag, args::Help,
  ticker_service::tick_service, std::atomic_bool, std::function;
namespace logger = spdlog;

// setup stacktracing
backward::SignalHandling sh;

// handlers for graceful shutdown on ctrl-c
function<void(int)> shutdown_handler;
def signal_handler(int signal) -> void { shutdown_handler(signal); }

def main(i16 argc, c_str argv[]) -> i16 {
  // setup the parser
  mutant parser = ArgumentParser("Websocket and ZeroMQ tick replicator");
  let help = HelpFlag(parser, "help", "Display this help menu", {'h', "help"});
  mutant zbind
    = ValueFlag<str>(parser, "zbind", "Publisher queue Uri", {'z', "zbind"}, "tcp://*:9000");
  mutant name
    = ValueFlag<str>(parser, "exchange_name", "Exchange name", {'e', "exchange"}, "kraken");

  // parse the cli args
  try {
    parser.ParseCLI(argc, argv);
  } catch (Help&) {
    cout << parser;
    return 0;
  }

  // check if the exchange name maps
  let exid = exchange_name::as_enum(name.Get());
  if (exid == null) {
    logger::error("Unsupported exchange: {}", name.Get());
    cout << parser;
    return 1;
  }

  // setup the cancellation token for the service to catch console ctrl-c
  mutant cancellation_token = atomic_bool(true);
  shutdown_handler = [&cancellation_token](int a) -> void {
    logger::info("Got signal: {}", a);
    cancellation_token = false;
  };
  signal(SIGINT, signal_handler);

  // launch the service
  try {
    logger::info("using exchange [{}] and publisher binding [{}] ", name.Get(), zbind.Get());
    async(launch::async, tick_service, exid.value(), zbind.Get(), cref(cancellation_token)).get();
  } catch (Exception e) {
    logger::error(e.what());
    return 1;
  }
  return 0;
}
