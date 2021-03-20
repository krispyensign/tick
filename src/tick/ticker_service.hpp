#pragma once
#include "kraken_tick.hpp"

#include "../common/macros.hpp"

namespace ticker_service {
def tick_service(exchange_name ex_name, String zbind, AtomicBool is_running) -> void;
}
#include "../common/unmacros.hpp"
