#pragma once
#include "kraken_exchange.hpp"
#include "tick.hpp"

namespace ticker_service {
def tick_service(exchange_name ex_name, String zbind, AtomicBool is_running)->void;
}
