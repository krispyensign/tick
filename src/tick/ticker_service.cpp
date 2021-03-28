#include "ticker_service.hpp"

#include "../deps/encdec.hpp"
#include "../deps/logger.hpp"
#include "../deps/websocket.hpp"
#include "../deps/zero.hpp"

#include <atomic>
#include <cpprest/ws_client.h>

#include "../common/macros.hpp"

namespace ticker_service {
using namespace std;

auto ws_send = [](websocket_callback_client* wsock, const str& data) -> void {
    auto envelope = websocket_outgoing_message();
    envelope.set_utf8_message(data);
    logger::info("created outmsg");
    try {
      wsock->send(envelope);
    } catch(Exception e) {
      logger::error("Failed to send for some reason :( {}", e.what());
      throw;
    }
};

auto tick_service(exchange_name ex_name, const str& zbind, const atomic_bool& is_running) -> void {
  // attempt to get the available pairs for websocket subscription
  constexpr const auto default_timer = 10ms;
  auto exi = [](exchange_name ex) -> exchange_interface {
    switch (ex.inner) {
      EXCHANGE_INF_CASE(kraken)
    default:
      throw error("unrecognized exchange");
    };
  }(ex_name);

  // setup publisher
  auto* ctx = new context_t(1);
  auto* publisher = new socket_t(*ctx, socket_type::pub);
  publisher->bind(zbind);
  logger::info("socket bound");

  auto* wsock = new websocket_callback_client();
  wsock->connect(exi.ws_uri).get();
  const auto sub_req = exi.create_tick_sub_request(exi.get_pairs_list());
  logger::info("got pairs");

  wsock->set_message_handler([&](WebSocketIncomingMessage data) -> void {
    if (is_running) {
      const auto msg = data.extract_string().get();
      if (const auto update = exi.parse_event(msg); update != null)
        publisher->send(zmq::message_t(Encoder::encode(update.value())),
                        zmq::send_flags::dontwait);
      else
        logger::info(msg);
    }
  });
  logger::info("callback setup.");

  ws_send(wsock, sub_req);
  logger::info("sent pairs");

  // loop until ctrl-c or forever
  logger::info("waiting for shutdown.");
  while (is_running) this_thread::sleep_for(default_timer);
  logger::info("got shutdown signal");

  // stop the work vent first
  ws_send(wsock, exi.create_tick_unsub_request());
  this_thread::sleep_for(default_timer);

  // then stop the sink
  publisher->close();
  ctx->shutdown();
  logger::info("shutdown complete");
  delete wsock;
  delete publisher;
  delete ctx;
}
} // namespace ticker_service
