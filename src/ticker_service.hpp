#ifndef ticker_service
#define ticker_service

#include "kraken.hpp"

let send_tick = $(MicroService ms, pair_price_update& event) -> bool {
  // log healthy after 4 good ticks
  ms->state.tick_count++;
  if (ms->state.tick_count == 4) fmt::print("Ticker is healthy. Waiting on shutdown.");

  // pack it up and send it
  mut buf = stringstream();
  msgpack::pack(buf, event);
  ms->ticker_z.send(zmq::message(buf.str()), zmq::send_flags::none);
  return true;
};

let process_tick = $(MicroService ms, str& msg) -> bool {
  // short-circuit if shutting down
  if (not ms->state.is_running) return false;

  // parse and dispatch result
  return type_switch(parse_event(msg))(
    // if it's a valid event then queue
    $$(pair_price_update& p) { return send_tick(ms, p); },

    // else log then eat the exception
    $(exception& e) {
      fmt::print(e.what());
      return false;
    });
};

let tick_service = $(MicroService ms) -> i16 {
  // attempt to get the available pairs for websocket subscription
  let pair_result = get_pairs_list(api_url, assets_path).get();

  // initialize publisher
  ms->context_z = zmq::context(1);
  ms->ticker_z = zmq::socket(ms->context_z, zmq::socket_type::pub);
  ms->ticker_z.bind(ms->config.zbind);

  // connect and setup handler
  ms->ticker_ws.set_message_handler(
    $$(rest::websocket_message msg) { process_tick(ms, msg.extract_string().get()); });
  ms->ticker_ws.connect(ms->config.ws_uri).get();

  // serialize the subscription request and send it off
  let subscription_json = create_tick_sub_request(pair_result).serialize();
  mut subscription_msg = rest::websocket_out_message();
  subscription_msg.set_utf8_message(subscription_json);
  ms->ticker_ws.send(subscription_msg);

  return 0;
};

#endif
