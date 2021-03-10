#ifndef ticker_service_hpp
#define ticker_service_hpp
#include "kraken.hpp"

namespace this_thread = std::this_thread;
namespace logger = spdlog;

using namespace std::chrono_literals;

using zmq::socket_t, zmq::context_t, zmq::message_t,
  web::websockets::client::websocket_callback_client,
  web::websockets::client::websocket_incoming_message, web::uri,
  web::websockets::client::websocket_outgoing_message, std::stringstream, std::atomic_bool,
  msgpack::pack, zmq::send_flags, zmq::socket_type;

namespace ticker_service {

let select_exchange = [](exchange_name ex) -> exchange_interface {
  switch (ex.inner) {
    case exchange_name::kraken:
      return {
        kraken_exchange::create_tick_sub_request,
        kraken_exchange::get_pairs_list,
        kraken_exchange::create_tick_unsub_request,
        kraken_exchange::parse_event,
        kraken_exchange::ws_uri,
      };
    default:
      throw error("unrecognized exchange");
  }
};

let send_tick = [](socket_t& ticker_publisher, const pair_price_update& event) -> void {
  // pack it up and send it
  mutant buf = stringstream();
  pack(buf, event);
  ticker_publisher.send(message_t(buf.str()), send_flags::none);
};

let ws_send = [](websocket_callback_client& ticker_ws, const str& in_msg) -> void {
  // create a utf-8 websocket envolope and send it
  mutant out_msg = websocket_outgoing_message();
  out_msg.set_utf8_message(in_msg);
  ticker_ws.send(out_msg).get();
};

let start_publisher = [](const str& zbind, context_t& ctx) -> socket_t {
  // get a publisher socket
  mutant publisher = socket_t(ctx, socket_type::pub);
  publisher.bind(zbind);
  return publisher;
};

let start_websocket = [](const function<str(const vec<str>&)>& create_tick_sub_request_callback,
                         const str& ws_uri,
                         const vec<str>& pair_result) -> websocket_callback_client {
  // connect sockets
  mutant wsock = websocket_callback_client();
  wsock.connect(ws_uri).get();
  logger::info("websocket connected");

  // serialize the subscription request and send it off
  let subscription = create_tick_sub_request_callback(pair_result);
  ws_send(wsock, subscription);
  logger::info("subscription sent: {}", subscription);
  return wsock;
};

let ws_handler = [](const atomic_bool& is_running,
                    socket_t& publisher,
                    const function<optional<pair_price_update>(const str&)>& parse_event)
  -> function<void(const websocket_incoming_message& data)> {
  return [&, is_healthy = false](const websocket_incoming_message& data) mutable {
    if (is_running) {
      let msg = data.extract_string().get();
      let update = parse_event(msg);

      if (update != null and not is_healthy) {
        is_healthy = true;
        logger::info("**ticker healthy**");
      }

      if (update != null)
        send_tick(publisher, update.value());
      else
        logger::info(msg);
    }
  };
};

let tick_service
  = [](exchange_name ex_name, const str& zbind, const atomic_bool& is_running) -> void {
  // configure exchange and perform basic validation on the config
  let exi = select_exchange(ex_name);
  if (not uri::validate(zbind) or uri(zbind).is_empty())
    throw error("Invalid binding uri for config");
  logger::info("conf validated");

  // attempt to get the available pairs for websocket subscription
  let pair_result = exi.get_pairs_list();
  logger::info("got pairs");

  // setup publisher
  mutant ctx = context_t(1);
  mutant publisher = start_publisher(zbind, ctx);
  logger::info("socket bound");

  // start websocket and setup callback for incoming messages
  mutant wsock = start_websocket(exi.create_tick_sub_request, exi.ws_uri, pair_result);
  wsock.set_message_handler(ws_handler(is_running, publisher, exi.parse_event));
  logger::info("callback setup. waiting for shutdown");
  while (is_running) this_thread::sleep_for(100ms);
  logger::info("got shutdown signal");

  // stop the work vent first
  ws_send(wsock, exi.create_tick_unsub_request());
  this_thread::sleep_for(100ms);
  wsock.close();

  // then stop the sink
  publisher.close();
  ctx.shutdown();
  logger::info("shutdown complete");
};

}  // namespace ticker_service

#endif
