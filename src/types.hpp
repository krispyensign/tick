#ifndef types_hpp
#define types_hpp

#include "common.hpp"

#pragma region std_includes
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
#pragma endregion

#pragma third_party_includes
#include <cpprest/http_client.h>
#include <cpprest/http_msg.h>
#include <cpprest/ws_client.h>
#include <cpprest/ws_msg.h>
#include <fmt/core.h>
#include <fmt/ranges.h>
#include <msgpack.hpp>
#include <rapidjson/document.h>
#include <rapidjson/rapidjson.h>
#include <zmq.hpp>
#pragma endregion

#define vec std::vector
#define var std::variant

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

using namespace std;

using error = std::runtime_error;

namespace json {
  using namespace rapidjson;
}

namespace rest {
  using methods = web::http::methods;
  using status_codes = web::http::status_codes;
  using client = web::http::client::http_client;
  using client_config = web::http::client::http_client_config;

  using websocket = web::websockets::client::websocket_callback_client;
  using websocket_message = web::websockets::client::websocket_incoming_message;
} // namespace rest

namespace zmq {
  using context = zmq::context_t;
  using message = zmq::message_t;
  using socket = zmq::socket_t;
} // namespace zmq

struct tick_sub_req {
  str event;
  vec<str> pairs;
  struct {
    str name;
  } subscription;
};

struct micro_service {
  zmq::socket ticker_z;
  rest::websocket ticker_ws;
  zmq::context context_z;
  struct {
    int tick_count = 0;
    bool is_running = false;
  } state;
  struct {
    str zbind;
    str ws_uri;
  } config;
};
typedef shared_ptr<micro_service> MicroService;

struct pair_price_update {
  str trade_name;
  double ask;
  double bid;
  MSGPACK_DEFINE(trade_name, ask, bid);
};
#endif
