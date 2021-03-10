#ifndef ticker_service_hpp
#define ticker_service_hpp
#include "kraken.hpp"
#include "types.hpp"

using namespace std;

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

let send_tick = [](zmq::socket_t& ticker_publisher, const pair_price_update& event) -> bool {
  // pack it up and send it
  mutant buf = stringstream();
  msgpack::pack(buf, event);
  ticker_publisher.send(zmq::message_t(buf.str()), zmq::send_flags::none);
  return true;
};

let ws_send = [](ws::client& ticker_ws, const str& in_msg) -> void {
  // create a utf-8 websocket envolope
  mutant out_msg = ws::out_message();
  out_msg.set_utf8_message(in_msg);

  // send it
  ticker_ws.send(out_msg).get();
};

let start_publisher = [](const str& zbind, zmq::context_t& ctx) -> zmq::socket_t {
  // get a publisher socket
  mutant publisher = zmq::socket_t(ctx, zmq::socket_type::pub);
  logger::info("socket provisioned");

  // attempt to bind
  publisher.bind(zbind);
  logger::info("socket bound");

  return publisher;
};

let start_websocket = [](const function<str(const vec<str>&)>& create_tick_sub_request_callback,
                         const str& ws_uri,
                         const vec<str>& pair_result) -> ws::client {
  // configure websocket client
  mutant ws_conf = ws::config();
  ws_conf.set_validate_certificates(false);
  mutant wsock = ws::client(ws_conf);
  logger::info("websocket provisioned");

  // connect sockets
  wsock.connect(ws_uri).get();
  logger::info("websocket connected");

  // serialize the subscription request and send it off
  let subscription = create_tick_sub_request_callback(pair_result);
  logger::info("sending subscription: {}", subscription);
  ws_send(wsock, subscription);

  return wsock;
};

let ws_handler = [](const atomic_bool& is_running,
                    zmq::socket_t& publisher,
                    const function<optional<pair_price_update>(const str&)>& parse_event)
  -> function<void(const ws::in_message& data)> {
  return [&, is_healthy = false](const ws::in_message& data) mutable {
    if (is_running) {
      let msg = data.extract_string().get();
      if (let update = parse_event(msg); update != null and not is_healthy) {
        is_healthy = true;
        logger::info("**ticker healthy**");
        send_tick(publisher, update.value());
      } else if (update != null and is_healthy)
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
  if (not web::uri::validate(zbind) or web::uri(zbind).is_empty())
    throw error("Invalid binding uri for config");
  logger::info("conf validated");

  // attempt to get the available pairs for websocket subscription
  let pair_result = exi.get_pairs_list();
  logger::info("got pairs");

  // provision all the endpoints and connections
  mutant ctx = zmq::context_t(1);
  mutant publisher = start_publisher(zbind, ctx);
  mutant wsock = start_websocket(exi.create_tick_sub_request, exi.ws_uri, pair_result);

  // setup callback for incoming messages
  wsock.set_message_handler(ws_handler(is_running, publisher, exi.parse_event));
  logger::info("callback setup");

  // periodically check if service is still alive
  logger::info("sleeping, waiting for shutdown");
  while (is_running) this_thread::sleep_for(100ms);

  // stop the work vent first
  logger::info("got shutdown signal");
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
