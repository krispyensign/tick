#ifndef types_hpp
#define types_hpp
#include "base_types.hpp"

#include <msgpack.hpp>
#include <cpprest/http_client.h>
#include <cpprest/http_msg.h>
#include <cpprest/ws_client.h>
#include <rapidjson/rapidjson.h>
#include <spdlog/spdlog.h>

#include <args.hxx>

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

struct service_config {
  str zbind;
  str ws_uri;
  str api_url;
  str assets_path;
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
  args::ValueFlag<str> ws_uri;
  args::ValueFlag<str> api_url;
  args::ValueFlag<str> asset_path;
};

#endif
