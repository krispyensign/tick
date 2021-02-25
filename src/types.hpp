#ifndef types_hpp
#define types_hpp
#pragma region system_includes

#include <exception>
#include <initializer_list>
#include <iterator>
#include <memory>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <variant>
#include <vector>
#include <functional>

#include <cpprest/http_client.h>
#include <cpprest/http_msg.h>
#include <cpprest/ws_client.h>
#include <cpprest/ws_msg.h>
#include <fmt/core.h>
#include <fmt/ranges.h>
#include <rapidjson/document.h>
#include <rapidjson/rapidjson.h>

#include <msgpack.hpp>
#include <zmq.hpp>

#include <spdlog/spdlog.h>
#pragma endregion

#pragma region base_type_aliases
typedef int i16;
typedef unsigned int u16;
typedef long i32;
typedef unsigned long u32;
typedef long long i64;
typedef unsigned long long u64;
typedef float f32;
typedef double f64;
typedef char *c_str;
typedef std::string str;
#pragma endregion

#pragma region namespace_aliases

using error = std::runtime_error;
using namespace std;

template <typename T>
using vec = vector<T, std::allocator<T>>;

template <typename... Ts>
using var = variant<Ts...>;

namespace logger {
  using namespace spdlog;
}

namespace json {
using namespace rapidjson;
}

namespace rest {
using methods = web::http::methods;
using status_codes = web::http::status_codes;
using client = web::http::client::http_client;
using config = web::http::client::http_client_config;
using response = web::http::http_response;

}  // namespace rest

namespace ws {
using client = web::websockets::client::websocket_callback_client;
using in_message = web::websockets::client::websocket_incoming_message;
using out_message = web::websockets::client::websocket_outgoing_message;
using config = web::websockets::client::websocket_client_config;
}


#pragma endregion

#pragma region data structures
enum exchange_name {
  KRAKEN,
};

struct service_config {
    str zbind;
    str ws_uri;
    str api_url;
    str assets_path;
};

struct service_state {
    int tick_count = 0;
    bool is_running = false;
};

struct pair_price_update {
  str trade_name;
  double ask;
  double bid;
  MSGPACK_DEFINE(trade_name, ask, bid);
};
#pragma endregion

#endif
