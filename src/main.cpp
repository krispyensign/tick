#pragma region includes
#include "common.hpp"

#include <algorithm>
#include <exception>
#include <iterator>
#include <stdexcept>
#include <string>
#include <utility>
#include <variant>
#include <vector>
#include <memory>

#include <cpprest/http_client.h>
#include <cpprest/http_msg.h>
#include <cpprest/ws_client.h>
#include <cpprest/ws_msg.h>

#include <rapidjson/document.h>
#include <rapidjson/rapidjson.h>

#include <fmt/core.h>

#include <zmq.hpp>
#pragma endregion

#pragma region usings
using except = std::exception;
using std::runtime_error;
using str = std::string;
using std::vector;

using rapidjson::Document;
using rapidjson::Value;

auto &GET = web::http::methods::GET;
auto &OK = web::http::status_codes::OK;
using web::http::client::http_client;
using web::http::client::http_client_config;

using web::websockets::client::websocket_callback_client;
using web::websockets::client::websocket_incoming_message;

using fmt::format;
using fmt::print;

using zmq::context_t;
using zmq::socket_t;
#pragma endregion

#pragma region helpers
#define vec vector
#define var std::variant
#define publisher zmq::socket_type::pub

#pragma endregion

#pragma region dsl
#define lparen (
#define rparen )
#define lbrace {
#define rbrace }
#define if if lparen
#define for for lparen
#define then rparen lbrace
#define doloop rparen lbrace
#define endif rbrace
#define endfor rbrace
#define in :
#define ne not_eq
#pragma endregion
struct tick_sub_req {
  str event;
  vec<str> pairs;
  struct {
    str name;
  } subscription;
};

struct micro_service {
  socket_t ticker_z;
  websocket_callback_client ticker_ws;
  context_t context_z = context_t();
  bool is_running = false;
  int tick_count = 0;
  struct {
    str zbind;
    str ws_uri;
  } config;
};
typedef std::shared_ptr<micro_service> MicroService;

struct pair_price_update {
  str trade_name;
  double ask;
  double bid;
};

let create_tick_sub_request = $(vec<str>& pairs) -> tick_sub_req {
  return {
    .event = "subscribe",
    .pairs = pairs,
    .subscription = {
      .name = "ticker",
    },
  };
};


let get_pairs_list = $(str& api_url, str& assets_path) -> var<vec<str>, except> {
  // disable ssl configs for now
  mut config = http_client_config();
  config.set_validate_certificates(false);

  // make the call and get a response back
  let response = http_client(api_url, config).request(GET, assets_path).get();

  // if not OK then return an error
  if response.status_code() not_eq OK then
    return runtime_error("Returned " + to_string(response.status_code()));
  endif

  // extract the text from the response
  let response_text = response.extract_string().get();

  // attempt to parse the doc
  mut json_doc = Document();
  json_doc.Parse(response_text.c_str());
  if json_doc.HasParseError() or not json_doc.HasMember("result") then
    return runtime_error(format("Failed to parse returned document: {}", response_text.c_str()));
  endif

  // extract wsnames from the doc
  let obj = json_doc["result"].GetObject();
  mut pair_list = vec<str>();
  for let& pair in obj doloop
    if pair.value.HasMember("wsname") then
      pair_list.push_back(pair.value["wsname"].GetString());
    endif
  endfor

  return pair_list;
};

let is_ticker = $(Value& json) -> bool {
  // validate the payload has the following fields
  let required_members = {"a", "b", "c", "v", "p", "t", "l", "h", "o"};

  // check each member in the requried members list
  for let& memb in required_members doloop
    // break if its missing any of them
    if not json.HasMember(memb) then
      return false;
    endif
  endfor

  return true;
};

let parse_event = $(str& msg_data) -> var<pair_price_update, except> {
  mut msg = Document();
  msg.Parse(msg_data.c_str());

  // validate the event parsed and there were not errors on the message itself
  if msg.HasParseError() or msg.HasMember("errorMessage") then
    return runtime_error(msg["errorMessage"].GetString());
  endif

  // validate it is a kraken publication type.  all kraken publications are arrays of size 4
  if msg.Size() not_eq 4 then
    return runtime_error("Not a publication");
  endif

  // get the payload of the publication
  let payload = msg[1].GetObject();

  // validate the payload is a tick object
  if not is_ticker(payload) then
    return runtime_error("Payload is not a tick");
  endif

  // get the pair name of the publication
  let pair = msg[3].GetString();

  // construct a neutral format for the price update event
  return pair_price_update {
    .trade_name = pair,
    .ask = payload["a"][0].GetDouble(),
    .bid = payload["b"][0].GetDouble(),
  };
};

let process_tick = $(str& msg, MicroService ms) -> bool {
  // short-circuit if shutting down
  if not ms->is_running then
    return false;
  endif

  // attempt to parse the event
  let event = parse_event(msg);

  // log then eat any errors
  if is_a<except>(event) then
    print(get_as<except>(event).what());
    return false;
  endif


  // log healthy after 4 good ticks
  ms->tick_count++;
  if ms->tick_count == 4 then
    print("Ticker is healthy.  Waiting on shutdown.");
  endif

  return true;
};

let tick_service = $(MicroService ms) -> i16 {
  let pairResults = get_pairs_list(API_URL, ASSETS_PATH);
  ms->ticker_z = socket_t(ms->context_z, publisher);
  ms->ticker_z.bind(ms->config.zbind);
  ms->ticker_ws.connect(ms->config.ws_uri).get();
  ms->ticker_ws.set_message_handler([ms](websocket_incoming_message msg) {
    process_tick(msg.extract_string().get(), ms);
  });
  return 0;
};

def main(i16 argc, c_str argv[]) -> i16 {
  let ms = std::make_shared<micro_service>();
  return tick_service(ms);
}
