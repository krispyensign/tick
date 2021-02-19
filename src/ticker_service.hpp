#ifndef ticker_service
#define ticker_service

#include "kraken.hpp"

let send_tick = $(ServiceState s, ZSocket ticker_z, pair_price_update& event) -> bool {
  // log healthy after 4 good ticks
  s->tick_count++;
  if (s->tick_count == 4) fmt::print("Ticker is healthy. Waiting on shutdown.");

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
      fmt::print(e.what());
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
  validate(conf);
  let pair_result = get_pairs_list(api_url, assets_path).get();

  // initialize publisher
  mut context_z = zmq::context(1);
  mut ticker_z = make_shared<zmq::socket_t>(context_z, zmq::socket_type::pub);
  ticker_z->bind(conf.zbind);

  // connect and setup websocket handler
  mut ticker_ws = make_shared<rest::websocket>();
  mut st = make_shared<state>();
  ticker_ws->set_message_handler(
    $$(rest::websocket_message msg) { process_tick(st, ticker_z, msg.extract_string().get()); });
  ticker_ws->connect(conf.ws_uri).get();

  // serialize the subscription request and send it off
  let subscription_json = create_tick_sub_request(pair_result).serialize();
  ws_send(ticker_ws, subscription_json);

  return;
};

#endif
