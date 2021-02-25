#ifndef ticker_service_hpp
#define ticker_service_hpp
#include "types.hpp"

namespace ticker_service {
auto tick_service(exchange_name ex, const service_config& conf) -> tuple<function<void(void)>,pplx::task<void>>;
}

#endif
