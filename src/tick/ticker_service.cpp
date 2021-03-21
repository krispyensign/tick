#include "ticker_service.hpp"

#include "../deps/encdec.hpp"
#include "../deps/logger.hpp"
#include "../deps/websocket.hpp"
#include "../deps/zero.hpp"

#include "../common/macros.hpp"
#include "zmq.hpp"
#include <memory>

namespace ticker_service {
using std::shared_ptr;
constexpr let default_timer = 10ms;

def select_exchange(exchange_name ex) -> exchange_interface {
  switch (ex.inner) {
    EXCHANGE_INF_CASE(kraken)
    default:
      throw error("unrecognized exchange");
  };
}

def tick_service(
  exchange_name ex_name,
  String zbind,
  AtomicBool is_running
) -> void {
  // attempt to get the available pairs for websocket subscription
  let exi = ticker_service::select_exchange(ex_name);
  let pair_result = exi.get_pairs_list();
  logger::info("got pairs");

  // setup publisher
  let ctx = make_shared<context_t>(1);
  let publisher = make_shared<socket_t>(*ctx, socket_type::pub);
  publisher->bind(zbind);
  logger::info("socket bound");

  // start websocket and setup callback for incoming messages
  let wsock = make_websocket(exi.ws_uri);
  let subscription = exi.create_tick_sub_request(pair_result);
  wsock->send(subscription);
  logger::info("subscription sent: {}", subscription);
  wsock->set_message_handler(
    [&](WebSocketIncomingMessage data) {
    if (is_running) {
      let msg = data.extract_string().get();
      if (let update = exi.parse_event(msg); update != null) {
        publisher->send(zmq::message_t(Encoder::encode(update.value())), zmq::send_flags::dontwait);
      } else {
        logger::info(msg);
      }}}
    );
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
  publisher->close();
  ctx->shutdown();
  logger::info("shutdown complete");
}
}  // namespace ticker_service
