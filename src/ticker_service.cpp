#include "kraken.hpp"
#include "templates.hpp"
#include "types.hpp"

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

auto send_tick(zmq::socket_t& ticker_publisher, const pair_price_update& event) -> bool {
  // pack it up and send it
  auto buf = stringstream();
  msgpack::pack(buf, event);
  ticker_publisher.send(zmq::message_t(buf.str()), zmq::send_flags::none);
  return true;
};

auto process_tick(zmq::socket_t& ticker_publisher, const str& incoming_msg) -> bool {
  // parse and dispatch result
  return type_match(
    exchange::parse_event(incoming_msg),
    // if it's a valid event then queue
    [&ticker_publisher](const pair_price_update& p) { return send_tick(ticker_publisher, p); },

    // else log then eat the exception
    [](const exception& e) {
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

auto ws_send(ws::client& ticker_ws, const str& in_msg) -> void {
  // create a websocket envolope
  auto out_msg = ws::out_message();
  // add the message in
  out_msg.set_utf8_message(in_msg);
  // send it
  ticker_ws.send(out_msg).get();
};

auto start_publisher(const service_config& conf, zmq::context_t& ctx) -> zmq::socket_t {
  // get a publisher socket
  auto publisher = zmq::socket_t(ctx, zmq::socket_type::pub);
  logger::info("socket provisioned");

  // attempt to bind
  publisher.bind(conf.zbind);
  logger::info("socket bound");

  return publisher;
};

auto start_websocket(const service_config& conf) -> ws::client {
  // configure websocket client
  auto ws_conf = ws::config();
  ws_conf.set_validate_certificates(false);
  auto wsock = ws::client(ws_conf);
  logger::info("websocket provisioned");

  // connect sockets
  wsock.connect(conf.ws_uri).get();
  logger::info("websocket connected");
  return wsock;
};

auto tick_service(exchange_name ex, const service_config& conf) -> void {
  // configure exchange and perform basic validation on the config
  select_exchange(ex);
  validate(conf);
  logger::info("conf validated");

  // attempt to get the available pairs for websocket subscription
  const auto pair_result = exchange::get_pairs_list(conf.api_url, conf.assets_path);
  logger::info("got pairs");

  // provision all the endpoints and connections
  auto ctx = zmq::context_t(1);
  auto publisher = start_publisher(conf, ctx);
  auto wsock = start_websocket(conf);

  // serialize the subscription request and send it off
  ws_send(wsock, exchange::create_tick_sub_request(pair_result).serialize());

  // setup message callback
  auto tick_count = 0;
  auto is_running = false;
  wsock.set_message_handler([&is_running, &publisher, &tick_count](const ws::in_message data) {
    data.extract_string()
      .then([&is_running](const str& msg) {
        return is_running ? msg : throw error("Caught shutdown.");
      })
      .then([&publisher](const str& msg) { return process_tick(publisher, msg) ? 1 : 0; })
      .then([&tick_count](const int result) {
        if (tick_count += result == 4) logger::info("Ticker healthy.");
      })
      .get();
  });
  logger::info("callback setup");

  return;
};
}  // namespace ticker_service
