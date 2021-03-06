#pragma once
#include "kraken_tick.hpp"

namespace ticker_service {

using web::uri;

constexpr const auto default_timer = 100ms;

namespace {
let select_exchange = [](exchange_name ex) -> exchange_interface {
  switch (ex.inner) { EXCHANGE_INF_CASE(kraken) default : throw error("unrecognized exchange"); }
};

let ws_handler = [](AtomicBool is_running,
                    Publisher publisher,
                    const function<optional<pair_price_update>(String)>& parse_event)
  -> function<void(WebSocketIncomingMessage data)> {
  return [&, is_healthy = false](WebSocketIncomingMessage data) mutable {
    if (is_running) {
      let msg = data.extract_string().get();
      if (let update = parse_event(msg); update != null and not is_healthy) {
        is_healthy = true;
        logger::info("**ticker healthy**");
        publisher->send(Encoder::encode(msg));
      } else if (update != null and is_healthy) {
        publisher->send(Encoder::encode(msg));
      } else {
        logger::info(msg);
      }
    }
  };
};
}  // namespace

let tick_service = [](exchange_name ex_name, String zbind, AtomicBool is_running) -> void {
  // configure exchange and perform basic validation on the config
  let exi = select_exchange(ex_name);
  if (not uri::validate(zbind) or uri(zbind).is_empty()) {
    throw error("Invalid binding uri for config");
  }
  logger::info("conf validated");

  // attempt to get the available pairs for websocket subscription
  let pair_result = exi.get_pairs_list();
  logger::info("got pairs");

  // setup publisher
  let ctx = make_context(1);
  let publisher = make_publisher(ctx, zbind);
  logger::info("socket bound");

  // start websocket and setup callback for incoming messages
  let wsock = make_websocket(exi.ws_uri);
  let subscription = exi.create_tick_sub_request(pair_result);
  wsock->send(subscription);
  logger::info("subscription sent: {}", subscription);
  wsock->set_message_handler(ws_handler(is_running, publisher, exi.parse_event));
  logger::info("callback setup. waiting for shutdown");

  // loop until ctrl-c or forever
  while (is_running) {
    this_thread::sleep_for(default_timer);
  }
  logger::info("got shutdown signal");

  // stop the work vent first
  wsock->send(exi.create_tick_unsub_request());
  this_thread::sleep_for(default_timer);

  // then stop the sink
  ctx->shutdown();
  logger::info("shutdown complete");
};
}  // namespace ticker_service
