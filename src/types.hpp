#ifndef types_hpp
#define types_hpp
#include <range/v3/all.hpp>

#include <cpprest/http_client.h>
#include <cpprest/http_msg.h>
#include <cpprest/ws_client.h>
#include <rapidjson/document.h>
#include <rapidjson/rapidjson.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <spdlog/spdlog.h>

#include <args.hxx>
#include <atomic>
#include <backward.hpp>
#include <functional>
#include <future>
#include <memory>
#include <msgpack.hpp>
#include <thread>
#include <zmq.hpp>

#include "base_types.hpp"

using std::function;
using std::optional;

#define let const auto
#define mutant auto

namespace ws {
using client = web::websockets::client::websocket_callback_client;
using in_message = web::websockets::client::websocket_incoming_message;
using out_message = web::websockets::client::websocket_outgoing_message;
using config = web::websockets::client::websocket_client_config;
}  // namespace ws

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

enum exchange_name {
  kraken,
};

struct pair_price_update {
  str trade_name;
  double ask;
  double bid;
  MSGPACK_DEFINE(trade_name, ask, bid)
};

struct cli_config {
  args::HelpFlag help;
  args::ValueFlag<str> zbind;
  args::ValueFlag<exchange_name> exchange;
};

struct exchange_interface {
  function<str(const vec<str>&)> create_tick_sub_request;
  function<vec<str>(void)> get_pairs_list;
  function<str(void)> create_tick_unsub_request;
  function<optional<pair_price_update>(const str&)> parse_event;
  const str ws_uri;
};

#endif
