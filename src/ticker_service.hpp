#ifndef ticker_service_hpp
#define ticker_service_hpp
#include "types.hpp"

namespace ticker_service {
auto tick_service(
  const exchange_name& ex, const service_config& conf, const atomic_bool& is_running) -> void;
}

#endif
