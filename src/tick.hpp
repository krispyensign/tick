#pragma once
#include <future>

#include "deps/args.hpp"
#include "deps/backward.hpp"
#include "deps/http.hpp"
#include "deps/logger.hpp"
#include "ticker_service.hpp"
//clang-format off
#include "macros.hpp"
//clang-format on

using std::cout, std::launch, ticker_service::tick_service, std::atomic_bool;

// setup stacktracing
backward::SignalHandling sh;
