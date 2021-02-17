#ifndef ticker_service
#define ticker_service

#include "kraken.hpp"

let send_tick = $(MicroService ms, pair_price_update& event) -> bool {
  // log healthy after 4 good ticks
  ms->state.tick_count++;
  if (ms->state.tick_count == 4) fmt::print("Ticker is healthy.  Waiting on shutdown.");

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
  return type_match(parse_event(msg), exhaustive {
    // if it's a valid event then queue
    $$(pair_price_update p) -> bool { return send_tick(ms, p); },

    // else log then eat the exception
    $(exception e) -> bool { fmt::print(e.what()); return false; }});
};

let tick_service = $(MicroService ms) -> i16 {
  let pairResults = get_pairs_list(api_url, assets_path);
  ms->context_z = zmq::context(1);
  ms->ticker_z = zmq::socket(ms->context_z, zmq::socket_type::pub);
  ms->ticker_z.bind(ms->config.zbind);
  ms->ticker_ws.connect(ms->config.ws_uri).get();
  //TODO: create subscription message
  ms->ticker_ws.set_message_handler(
    $$(rest::websocket_message msg) { process_tick(ms, msg.extract_string().get()); });
  return 0;
};

#endif
