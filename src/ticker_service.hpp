#ifndef ticker_service
#define ticker_service

#include <spdlog/spdlog.h>

#include "kraken.hpp"


let send_tick = $(ServiceState s, ZSocket ticker_z, pair_price_update& event) -> bool {
  // log healthy after 4 good ticks
  s->tick_count++;
  if (s->tick_count == 4) spdlog::info("Ticker is healthy. Waiting on shutdown.");

  // pack it up and send it
  mut buf = stringstream();
  msgpack::pack(buf, event);
  ticker_z->send(zmq::message(buf.str()), zmq::send_flags::none);
  return true;
};

let process_tick = $(ServiceState s, ZSocket ticker_z, str& msg) -> bool {
  // short-circuit if shutting down
  if (not s->is_running) return false;

  // parse and dispatch result
  return type_switch(parse_event(msg))(
    // if it's a valid event then queue
    $$(pair_price_update& p) { return send_tick(s, ticker_z, p); },

    // else log then eat the exception
    $(exception& e) {
      spdlog::info(e.what());
      return false;
    });
};

let validate = $(config& conf) -> bool {
  if (not web::uri().validate(conf.ws_uri) or web::uri(conf.ws_uri).is_empty()) {
    throw error("Invalid websocket uri for config");
  }

  if (not web::uri().validate(conf.zbind) or web::uri(conf.zbind).is_empty()) {
    throw error("Invalid binding uri for config");
  }
  return true;
};

let ws_send = $(WebSocket ticker_ws, str& msg) -> void {
  // create a websocket envolope
  mut ws_msg = rest::websocket_out_message();
  // add the message in
  ws_msg.set_utf8_message(msg);
  // send it
  ticker_ws->send(ws_msg).get();
};

let tick_service = $(config& conf) -> void {
  // attempt to get the available pairs for websocket subscription
  try {
    validate(conf);
  } catch(const exception& e) {
    spdlog::error(e.what());
    throw e;
  }
  spdlog::info("conf validated");

  vec<str> pair_result;
  try {
    pair_result = get_pairs_list(api_url, assets_path).get();
  } catch(const exception& e) {
    spdlog::error(e.what());
    throw e;
  }
  spdlog::info("got pairs");

  // initialize publisher
  mut context_z = zmq::context_t(1);
  mut sock = zmq::socket_t(context_z, zmq::socket_type::pub);
  mut ticker_z = make_shared<zmq::socket_t>(move(sock));
  spdlog::info("got socket");
  try {
    ticker_z->bind(conf.zbind);
  } catch(const exception& e) {
    spdlog::error(e.what());
    throw e;
  }
  spdlog::info("socket bound");

  // connect and setup websocket handler
  mut ticker_ws = make_shared<rest::websocket>();
  mut st = make_shared<state>();
  spdlog::info("got websockets");
  try {
    ticker_ws->set_message_handler(
      $$(rest::websocket_message msg) { process_tick(st, ticker_z, msg.extract_string().get()); });
    ticker_ws->connect(conf.ws_uri).get();
  } catch(const exception& e) {
    spdlog::error(e.what());
    throw e;
  }
  spdlog::info("websocket connected");

  // serialize the subscription request and send it off
  let subscription_json = create_tick_sub_request(pair_result).serialize();
  ws_send(ticker_ws, subscription_json);

  return;
};

#endif
