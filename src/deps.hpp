#ifndef deps_hpp
#define deps_hpp
#define FMT_HEADER_ONLY 1
#pragma region ranges
#include <range/v3/algorithm/all_of.hpp>
#include <range/v3/range/conversion.hpp>
#include <range/v3/view/filter.hpp>
#include <range/v3/view/transform.hpp>
#pragma endregion

#include <cpprest/http_client.h>
#include <cpprest/ws_client.h>
#include <fmt/format.h>
#include <rapidjson/document.h>
#include <rapidjson/rapidjson.h>
#include <spdlog/spdlog.h>

#include <args.hxx>
#include <backward.hpp>
#include <future>
#include <zmq.hpp>

#include "base_types.hpp"
#include "types.hpp"
#endif
