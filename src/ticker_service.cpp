#include <thread>

#include <cpprest/ws_client.h>
#include <zmq.hpp>

#include "kraken.hpp"
#include "templates.hpp"
#include "ns_helper.hpp"

namespace ticker_service {

struct exchange_interface {
  function<str(const vec<str>&)> create_tick_sub_request;
  function<vec<str>(const str&, const str&)> get_pairs_list;
  function<var<pair_price_update, str>(const str&)> parse_event;
  function<str(void)> create_tick_unsub_request;
};

auto select_exchange(exchange_name ex) -> exchange_interface {
  switch (ex) {
    case KRAKEN:
      return {
        .create_tick_sub_request = kraken::create_tick_sub_request,
        .get_pairs_list = kraken::get_pairs_list,
        .parse_event = kraken::parse_event,
        .create_tick_unsub_request = kraken::create_tick_unsub_request,
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

auto process_tick(const exchange_interface& ex, zmq::socket_t& ticker_publisher, const str& incoming_msg) -> bool {
  // parse and dispatch result
  return type_match(
    ex.parse_event(incoming_msg),
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

auto start_websocket(const exchange_interface& ex, const service_config& conf, const vec<str>& pair_result) -> ws::client {
  // configure websocket client
  auto ws_conf = ws::config();
  ws_conf.set_validate_certificates(false);
  auto wsock = ws::client(ws_conf);
  logger::info("websocket provisioned");

  // connect sockets
  wsock.connect(conf.ws_uri).get();
  logger::info("websocket connected");

  // serialize the subscription request and send it off
  auto subscription = ex.create_tick_sub_request(pair_result);
  logger::info("sending subscription:");
  logger::info(subscription);

  ws_send(wsock, subscription);

  return wsock;
}

auto tick_shutdown(const exchange_interface& ex, zmq::context_t& ctx, zmq::socket_t& publisher, ws::client& wsock) -> void {
  // stop the work vent first
  logger::info("Got shutdown signal");
  ws_send(wsock, ex.create_tick_unsub_request());
  this_thread::sleep_for(100ms);
  wsock.close();

  // then stop the sink
  publisher.close();
  ctx.shutdown();
  logger::info("Shutdown complete");
}

auto tick_service(exchange_name ex_name, const service_config& conf, const atomic_bool& is_running)
  -> void {
  // configure exchange and perform basic validation on the config
  auto ex = select_exchange(ex_name);
  validate(conf);
  logger::info("conf validated");

  // attempt to get the available pairs for websocket subscription
  const auto pair_result = ex.get_pairs_list(conf.api_url, conf.assets_path);
  logger::info("got pairs");

  // provision all the endpoints and connections
  auto ctx = zmq::context_t(1);
  auto publisher = start_publisher(conf, ctx);
  auto wsock = start_websocket(ex, conf, pair_result);

  // setup message callback
  auto tick_count = 0;
  wsock.set_message_handler([ex, &is_running, &publisher, &tick_count](const ws::in_message data) {
    if (is_running)
      data.extract_string()
        .then([ex, &publisher](const str& msg) { return process_tick(ex, publisher, msg); })
        .then([&tick_count](bool result) {
          if(result and tick_count < 4) ++tick_count;
          if (tick_count == 4) {
            logger::info("Ticker healthy.");
            tick_count++;
          }
        })
        .get();
  });
  logger::info("callback setup... sleeping forever");

  // periodically check if service is still alive then try to shutdown cleanly
  while (is_running) this_thread::sleep_for(100ms);
  tick_shutdown(ex, ctx, publisher, wsock);
  return;
}

}  // namespace ticker_service
