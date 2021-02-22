#include <exception>
#include <functional>
#include <ios>

#include "kraken.hpp"
#include "templates.hpp"
#include "types.hpp"
#include "zmq.hpp"

namespace exchange {
function<tick_sub_req(const vec<str>&)> create_tick_sub_request;
function<vec<str>(const str&, const str&)> get_pairs_list;
function<var<pair_price_update, std::exception>(const str&)> parse_event;
}  // namespace exchange

namespace ticker_service {
auto select_exchange(exchange_name ex) -> void {
  switch (ex) {
    case KRAKEN:
      exchange::create_tick_sub_request = kraken::create_tick_sub_request;
      exchange::get_pairs_list = kraken::get_pairs_list;
      exchange::parse_event = kraken::parse_event;
  }
};

auto send_tick(service_state& state, zmq::socket_t& ticker_publisher, const pair_price_update& event)
  -> bool {
  // log healthy after 4 good ticks
  state.tick_count++;
  if (state.tick_count == 4) logger::info("Ticker is healthy. Waiting on shutdown.");

  // pack it up and send it
  auto buf = stringstream();
  msgpack::pack(buf, event);
  ticker_publisher.send(zmq::message_t(buf.str()), zmq::send_flags::none);
  return true;
};

auto process_tick(service_state& state, zmq::socket_t& ticker_publisher, const str& incoming_msg) -> bool {
  // short-circuit if shutting down
  if (not state.is_running) return false;

  // parse and dispatch result
  return type_switch(exchange::parse_event(incoming_msg))(
    // if it's a valid event then queue
    [&](const pair_price_update& p) { return send_tick(state, ticker_publisher, p); },

    // else log then eat the exception
    [=](exception& e) {
      logger::info(e.what());
      return false;
    });
};

auto validate(const service_config& conf) -> bool {
  if (not web::uri().validate(conf.ws_uri) or web::uri(conf.ws_uri).is_empty()) {
    throw error("Invalid websocket uri for config");
  }

  if (not web::uri().validate(conf.zbind) or web::uri(conf.zbind).is_empty()) {
    throw error("Invalid binding uri for config");
  }
  return true;
};

auto ws_send(websocket::client& ticker_ws, const str& msg) -> void {
  // create a websocket envolope
  auto ws_msg = websocket::out_message();
  // add the message in
  ws_msg.set_utf8_message(msg);
  // send it
  ticker_ws.send(ws_msg).get();
};

auto start_publisher(const service_config& conf, zmq::context_t& ctx) -> zmq::socket_t {
  // get a publisher socket
  auto publisher = zmq::socket_t(ctx, zmq::socket_type::pub);
  logger::info("socket provisioned");

  // attempt to bind
  publisher.bind(conf.zbind);
  logger::info("socket bound");

  return move(publisher);
};

auto start_websocket(const service_config& conf) -> websocket::client {
  // configure websocket client
  auto ws_conf = websocket::config();
  ws_conf.set_validate_certificates(false);
  auto ws = websocket::client(ws_conf);
  logger::info("websocket provisioned");

  // connect sockets
  ws.connect(conf.ws_uri).get();
  logger::info("websocket connected");
  return move(ws);
};

auto tick_service(const service_config& conf) -> void {
  select_exchange(exchange_name::KRAKEN);
  // perform basic validation on the config
  validate(conf);
  logger::info("conf validated");

  // attempt to get the available pairs for websocket subscription
  const auto pair_result = exchange::get_pairs_list(conf.api_url, conf.assets_path);
  logger::info("got pairs");

  auto ctx = zmq::context_t(1);
  auto publisher = start_publisher(conf, ctx);
  auto ws = start_websocket(conf);

  // serialize the subscription request and send it off
  const auto subscription_json = exchange::create_tick_sub_request(pair_result).serialize();
  ws_send(ws, subscription_json);

  // setup message callback
  auto st = service_state();
  ws.set_message_handler([&](const websocket::in_message data) {
    data.extract_string().then([&](const str& msg) { process_tick(st, publisher, msg); });
  });
  logger::info("callback setup");

  return;
};
}  // namespace ticker_service
