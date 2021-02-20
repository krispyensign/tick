#ifndef ticker_service
#define ticker_service

#include "kraken.hpp"

let send_tick = $(ServiceState s, ZSocket ticker_z, pair_price_update& event)->bool {
  // log healthy after 4 good ticks
  s->tick_count++;
  if (s->tick_count == 4) spdlog::info("Ticker is healthy. Waiting on shutdown.");

  // pack it up and send it
  mut buf = stringstream();
  msgpack::pack(buf, event);
  ticker_z->send(zmq::message(buf.str()), zmq::send_flags::none);
  return true;
};

let process_tick = $(ServiceState s, ZSocket ticker_z, str& msg)->bool {
  // short-circuit if shutting down
  if (not s->is_running) return false;

  // parse and dispatch result
  return type_switch(parse_event(msg))(
    // if it's a valid event then queue
    $$(pair_price_update & p) { return send_tick(s, ticker_z, p); },

    // else log then eat the exception
    $(exception & e) {
      spdlog::info(e.what());
      return false;
    });
};

let validate = $(config& conf)->bool {
  if (not web::uri().validate(conf.ws_uri) or web::uri(conf.ws_uri).is_empty()) {
    throw error("Invalid websocket uri for config");
  }

  if (not web::uri().validate(conf.zbind) or web::uri(conf.zbind).is_empty()) {
    throw error("Invalid binding uri for config");
  }
  return true;
};

let ws_send = $(WebSocket ticker_ws, str& msg)->void {
  // create a websocket envolope
  mut ws_msg = websocket::out_message();
  // add the message in
  ws_msg.set_utf8_message(msg);
  // send it
  ticker_ws->send(ws_msg).get();
};

let get_websocket = $(ZSocket zpublisher, config& conf)->WebSocket {
  // configure websocket client
  mut ws_conf = websocket::client_config();
  ws_conf.set_validate_certificates(false);
  mut ws = make_shared<websocket::client>(ws_conf);

  // connect sockets
  ws->connect(conf.ws_uri).get();
  spdlog::info("websocket connected");

  return ws;
};

let tick_service = $(config& conf)->void {
  // perform basic validation on the config
  validate(conf);
  spdlog::info("conf validated");

  // attempt to get the available pairs for websocket subscription
  vec<str> pair_result;
  pair_result = get_pairs_list(api_url, assets_path).get();
  spdlog::info("got pairs");

  // get a publisher socket
  mut ctx = zmq::context(1);
  mut sock = zmq::socket(ctx, zmq::socket_type::pub);
  mut publisher = make_shared<zmq::socket>(move(sock));
  spdlog::info("socket provisioned");

  // attempt to bind
  publisher->bind(conf.zbind);
  spdlog::info("socket bound");

  // connect websocket
  mut ws = get_websocket(publisher, conf);

  // serialize the subscription request and send it off
  let subscription_json = create_tick_sub_request(pair_result).serialize();
  ws_send(ws, subscription_json);

  // setup message callback
  mut st = make_shared<state>();
  ws->set_message_handler($$(websocket::in_message data) {
    data.extract_string().then($$(str & msg) { process_tick(st, publisher, msg); });
  });
  spdlog::info("callback setup");

  return;
};

#endif
