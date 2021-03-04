#include <thread>
#include <zmq.hpp>

#include "kraken.hpp"
#include "ns_helper.hpp"
#include "templates.hpp"

namespace ticker_service {

auto select_exchange(exchange_name ex)
  -> tuple<function<str(const vec<str>&)>, function<vec<str>(const str&, const str&)>,
           function<var<pair_price_update, str>(const str&)>, function<str(void)>> {
  switch (ex) {
    case KRAKEN:
      return {
        kraken::create_tick_sub_request,
        kraken::get_pairs_list,
        kraken::parse_event,
        kraken::create_tick_unsub_request,
      };
    default:
      return {
        kraken::create_tick_sub_request,
        kraken::get_pairs_list,
        kraken::parse_event,
        kraken::create_tick_unsub_request,
      };
  }
}

auto send_tick(zmq::socket_t& ticker_publisher, const pair_price_update& event) -> bool {
  // pack it up and send it
  auto buf = stringstream();
  msgpack::pack(buf, event);
  ticker_publisher.send(zmq::message_t(buf.str()), zmq::send_flags::none);
  return true;
}

auto process_tick(const function<var<pair_price_update, str>(const str&)>& parse_event_callback,
                  zmq::socket_t& ticker_publisher, const str& incoming_msg) -> bool {
  // parse and dispatch result
  return type_match(
    parse_event_callback(incoming_msg),
    // if it's a valid event then queue
    [&ticker_publisher](const pair_price_update& p) { return send_tick(ticker_publisher, p); },

    // else log then eat the exception
    [](const str& e) {
      logger::info(e);
      return false;
    });
}

auto validate(const service_config& conf) -> bool {
  if (not web::uri().validate(conf.ws_uri) or web::uri(conf.ws_uri).is_empty()) {
    throw error("Invalid websocket uri for config");
  }

  if (not web::uri().validate(conf.zbind) or web::uri(conf.zbind).is_empty()) {
    throw error("Invalid binding uri for config");
  }
  return true;
}

auto ws_send(ws::client& ticker_ws, const str& in_msg) -> void {
  // create a websocket envolope
  auto out_msg = ws::out_message();
  // add the message in
  out_msg.set_utf8_message(in_msg);
  // send it
  ticker_ws.send(out_msg).get();
}

auto start_publisher(const service_config& conf, zmq::context_t& ctx) -> zmq::socket_t {
  // get a publisher socket
  auto publisher = zmq::socket_t(ctx, zmq::socket_type::pub);
  logger::info("socket provisioned");

  // attempt to bind
  publisher.bind(conf.zbind);
  logger::info("socket bound");

  return publisher;
}

auto start_websocket(const function<str(const vec<str>&)>& create_tick_sub_request_callback,
                     const service_config& conf, const vec<str>& pair_result) -> ws::client {
  // configure websocket client
  auto ws_conf = ws::config();
  ws_conf.set_validate_certificates(false);
  auto wsock = ws::client(ws_conf);
  logger::info("websocket provisioned");

  // connect sockets
  wsock.connect(conf.ws_uri).get();
  logger::info("websocket connected");

  // serialize the subscription request and send it off
  auto subscription = create_tick_sub_request_callback(pair_result);
  logger::info("sending subscription:");
  logger::info(subscription);

  ws_send(wsock, subscription);

  return wsock;
}

auto ws_handler(const atomic_bool& is_running, zmq::socket_t& publisher,
                const function<var<pair_price_update, str>(const str&)>& parse_event_callback)
  -> function<void(const ws::in_message data)> {
  return [&, is_healthy = false](const ws::in_message data) mutable {
    if (is_running)
      data.extract_string()
        .then([&](const str& msg) {
          if (process_tick(parse_event_callback, publisher, msg) == true and is_healthy == false) {
            is_healthy = true;
            logger::info("**ticker healthy**");
          }
        })
        .get();
  };
}

auto tick_service(const exchange_name& ex_name, const service_config& conf,
                  const atomic_bool& is_running) -> void {
  // configure exchange and perform basic validation on the config
  const auto [create_tick_sub_request, get_pairs_list, parse_event, create_tick_unsub_request] =
    select_exchange(ex_name);

  validate(conf);
  logger::info("conf validated");

  // attempt to get the available pairs for websocket subscription
  const auto pair_result = get_pairs_list(conf.api_url, conf.assets_path);
  logger::info("got pairs");

  // provision all the endpoints and connections
  auto ctx = zmq::context_t(1);
  auto publisher = start_publisher(conf, ctx);
  auto wsock = start_websocket(create_tick_sub_request, conf, pair_result);

  wsock.set_message_handler(ws_handler(is_running, publisher, parse_event));
  logger::info("callback setup");

  // periodically check if service is still alive then try to shutdown cleanly
  logger::info("sleeping, waiting for shutdown");
  while (is_running) this_thread::sleep_for(100ms);

  // stop the work vent first
  logger::info("got shutdown signal");
  ws_send(wsock, create_tick_unsub_request());
  this_thread::sleep_for(100ms);
  wsock.close();

  // then stop the sink
  publisher.close();
  ctx.shutdown();
  logger::info("shutdown complete");
}

}  // namespace ticker_service
