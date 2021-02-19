#ifndef types
#define types
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
#pragma endregion

#pragma region dsl
#define let const auto
#define mut auto
#define def auto
#define in :

#define vec std::vector
#define var std::variant
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

namespace json {
using namespace rapidjson;
}

namespace rest {
using methods = web::http::methods;
using status_codes = web::http::status_codes;
using client = web::http::client::http_client;
using client_config = web::http::client::http_client_config;
using response = web::http::http_response;

using websocket = web::websockets::client::websocket_callback_client;
using websocket_message = web::websockets::client::websocket_incoming_message;
using websocket_out_message = web::websockets::client::websocket_outgoing_message;
}  // namespace rest

namespace zmq {
using context = zmq::context_t;
using message = zmq::message_t;
using socket = zmq::socket_t;
}  // namespace zmq
#pragma endregion

#pragma region structs
struct tick_sub_req {
  str event;
  vec<str> pairs;
  struct {
    str name;
  } subscription;
  def serialize() -> str {
    return fmt::format(R"EOF(
      {
        "event":{},
        "pairs":{},
        "subscription":{
          "name": {}
        }
      })EOF", event, pairs, subscription.name);
  }
};

struct config {
    str zbind;
    str ws_uri;
};

struct state {
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

#pragma region other_aliases
typedef shared_ptr<rest::websocket> WebSocket;
typedef shared_ptr<zmq::socket> ZSocket;
typedef shared_ptr<state> ServiceState;
#pragma endregion

#endif
