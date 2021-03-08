#include "ticker_service.hpp"

namespace ticker_service {

#define make_exchange(exname)                                                        \
  case exname:                                                                       \
    return {                                                                         \
      exname##_exchange::create_tick_sub_request, exname##_exchange::get_pairs_list, \
        exname##_exchange::create_tick_unsub_request,                                \
    }

#define make_exchange_parser(exname) \
  case exname:                       \
    return exname##_exchange::parse_event

auto select_exchange(exchange_name ex) -> tuple<
  function<str(const vec<str>&)>,
  function<vec<str>(const str&, const str&)>,
  function<str(void)>> {
  switch (ex) {
    make_exchange(kraken);
    default:
      throw error("unrecognized exchange");
  }
}

auto select_exchange_parser(exchange_name ex) -> function<optional<pair_price_update>(const str&)> {
  switch (ex) {
    make_exchange_parser(kraken);
    default:
      throw error("unrecognized exchange");
  }
}

auto send_tick(zmq::socket_t& ticker_publisher, const pair_price_update& event) -> bool {
  // pack it up and send it
  auto buf = stringstream();
  msgpack::pack(buf, event);
  ticker_publisher.send(zmq::message_t(buf.str()), zmq::send_flags::none);
  return true;
}

auto validate(const service_config& conf) -> bool {
  if (not web::uri::validate(conf.ws_uri) or web::uri(conf.ws_uri).is_empty()) {
    throw error("Invalid websocket uri for config");
  }

  if (not web::uri::validate(conf.zbind) or web::uri(conf.zbind).is_empty()) {
    throw error("Invalid binding uri for config");
  }

  return true;
}

auto ws_send(ws::client& ticker_ws, const str& in_msg) -> void {
  // create a utf-8 websocket envolope
  auto out_msg = ws::out_message();
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

auto start_websocket(
  const function<str(const vec<str>&)>& create_tick_sub_request_callback,
  const service_config& conf,
  const vec<str>& pair_result) -> ws::client {
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

auto ws_handler(
  const exchange_name& ex_name, const atomic_bool& is_running, zmq::socket_t& publisher)
  -> function<void(const ws::in_message data)> {
  return [&, is_healthy = false, parse_event = select_exchange_parser(ex_name)](
           const ws::in_message& data) mutable {
    if (is_running) {
      auto msg = data.extract_string().get();
      if (auto update = parse_event(msg); update != nullopt and is_healthy) {
        is_healthy = true;
        logger::info("**ticker healthy**");
        send_tick(publisher, update.value());
      } else {
        logger::info(msg);
      }
    }
  };
}

auto tick_service(
  const exchange_name& ex_name, const service_config& conf, const atomic_bool& is_running) -> void {
  // configure exchange and perform basic validation on the config
  const auto [create_tick_sub_request, get_pairs_list, create_tick_unsub_request] =
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

  // setup callback for incoming messages
  wsock.set_message_handler(ws_handler(ex_name, is_running, publisher));
  logger::info("callback setup");

  // periodically check if service is still alive
  logger::info("sleeping, waiting for shutdown");
  while (is_running) {
    std::this_thread::sleep_for(100ms);
  }

  // stop the work vent first
  logger::info("got shutdown signal");
  ws_send(wsock, create_tick_unsub_request());
  std::this_thread::sleep_for(100ms);
  wsock.close();

  // then stop the sink
  publisher.close();
  ctx.shutdown();
  logger::info("shutdown complete");
}

}  // namespace ticker_service
