#ifndef ticker_service_hpp
#define ticker_service_hpp
#include <atomic>
#include <thread>
#include <zmq.hpp>

#include "kraken.hpp"
#include "ns_helper.hpp"
#include "types.hpp"

using std::atomic_bool;
using std::function;
using std::nullopt;
using std::stringstream;
using std::tuple;
using namespace std::chrono_literals;

namespace ticker_service {
auto tick_service(
  const exchange_name& ex, const service_config& conf, const atomic_bool& is_running) -> void;
}

#endif
