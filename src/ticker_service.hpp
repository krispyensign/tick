#ifndef ticker_service_hpp
#define ticker_service_hpp

#include "kraken.hpp"
#include "zmq.hpp"

let process_tick = $(MicroService ms, str &msg)->bool {
  // short-circuit if shutting down
  if (!ms->state.is_running)
    return false;

  // attempt to parse the event
  let event_result = parse_event(msg);

  // log then eat any errors
  if (holds_alternative<exception>(event_result)) {
    fmt::print(get<exception>(event_result).what());
    return false;
  }
  let event = get<pair_price_update>(event_result);

  // log healthy after 4 good ticks
  ms->state.tick_count++;
  if (ms->state.tick_count == 4)
    fmt::print("Ticker is healthy.  Waiting on shutdown.");

  // pack it up and send it
  mut buf = stringstream();
  msgpack::pack(buf, event);
  ms->ticker_z.send(zmq::message(buf.str()), zmq::send_flags::none);

  return true;
};

let tick_service = $(MicroService ms)->i16 {
  let pairResults = get_pairs_list(api_url, assets_path);
  ms->context_z = zmq::context(1);
  ms->ticker_z = zmq::socket(ms->context_z, zmq::socket_type::pub);
  ms->ticker_z.bind(ms->config.zbind);
  ms->ticker_ws.connect(ms->config.ws_uri).get();
  ms->ticker_ws.set_message_handler([&ms](rest::websocket_message msg) {
    process_tick(ms, msg.extract_string().get());
  });
  return 0;
};

#endif
