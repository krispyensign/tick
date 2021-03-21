#pragma once
#define BACKWARD_HAS_LIBUNWIND 1
#include <future>

#include "../deps/args.hpp"
#include "../deps/backward.hpp"
#include "../deps/http.hpp"
#include "../deps/logger.hpp"
#include "ticker_service.hpp"

#include "../common/macros.hpp"

using std::cout, std::launch, ticker_service::tick_service, std::atomic_bool;

// setup stacktracing
backward::SignalHandling sh;
