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

using namespace std;

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

namespace rest {
  using web::http::methods;
  using web::http::status_codes;
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

struct pack_encoder {
  template <typename T> def encode(T t)->str {
    mut buffer = str();
    msgpack::pack(buffer, t);
    return buffer;
  }
};

class publisher {
private:
  zmq::socket_t sock;

public:
  publisher(zmq::context &ctx) {
    sock = zmq::socket(ctx, zmq::socket_type::pub);
  }

  template <typename T> def bind(T t)->void { sock.bind(t); }

  def send(const vec<str> &envelope)->void {
    mut msg = zmq::message(fmt::format("{}", fmt::join(envelope, " ")));
    sock.send(msg, zmq::send_flags::dontwait);
  };
};

struct tick_sub_req {
  str event;
  vec<str> pairs;
  struct {
    str name;
  } subscription;
};

struct micro_service {
  optional<publisher> ticker_z;
  rest::websocket ticker_ws;
  zmq::context context_z;
  bool is_running = false;
  int tick_count = 0;
  pack_encoder encoder;
  struct {
    str zbind;
    str ws_uri;
  } config;
};
typedef shared_ptr<micro_service> MicroService;

class pair_price_update {
public:
  str trade_name;
  double ask;
  double bid;
  MSGPACK_DEFINE(trade_name, ask, bid);
};
#endif
