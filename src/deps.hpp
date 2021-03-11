#ifndef deps_hpp
#define deps_hpp
#define FMT_HEADER_ONLY 1
#pragma region ranges
#include <range/v3/algorithm/all_of.hpp>
#include <range/v3/range/conversion.hpp>
#include <range/v3/view/filter.hpp>
#include <range/v3/view/transform.hpp>
#pragma endregion

#include <cpprest/http_client.h>
#include <cpprest/ws_client.h>
#include <fmt/format.h>
#include <rapidjson/document.h>
#include <rapidjson/rapidjson.h>
#include <spdlog/spdlog.h>

#include <args.hxx>
#include <backward.hpp>
#include <future>
#include <zmq.hpp>

#include "base_types.hpp"
#include "types.hpp"

typedef std::shared_ptr<zmq::context_t> Context;

struct _publisher {
 private:
  zmq::socket_t sock;

 public:
  _publisher(Context ctx, const str& bind_addr) {
    sock = zmq::socket_t(*ctx, zmq::socket_type::pub);
    sock.bind(bind_addr);
  }
  ~_publisher() { sock.close(); }
  auto send(const str& msg) -> void { sock.send(zmq::message_t(msg), zmq::send_flags::none); }
};
typedef std::shared_ptr<_publisher> Publisher;

inline auto make_publisher(Context ctx, const str& bind_addr) -> Publisher {
  return std::make_shared<_publisher>(ctx, bind_addr);
}

inline auto make_context(int threads) -> Context {
  return std::make_shared<zmq::context_t>(threads);
}

struct Encoder {
  static auto encode(const str& enc) -> str {
    auto buf = std::stringstream();
    msgpack::pack(buf, enc);
    return buf.str();
  }
};

using WebSocketIncomingMessage = const web::websockets::client::websocket_incoming_message&;

struct _websocket {
 private:
  web::websockets::client::websocket_callback_client client;

 public:
  _websocket(const str& address) { client.connect(address).get(); }
  ~_websocket() { client.close(); }

  auto send(const str& msg) -> void {
    auto out_msg = web::websockets::client::websocket_outgoing_message();
    out_msg.set_utf8_message(msg);
    client.send(out_msg).get();
  }
  template <typename T>
  auto set_message_handler(T t) -> void {
    client.set_message_handler(t);
  }
};
typedef std::shared_ptr<_websocket> WebSocket;

inline auto make_websocket(const str& address) -> WebSocket {
  return make_shared<_websocket>(address);
}

inline auto make_json(const str& data) -> rapidjson::Document {
  auto doc = rapidjson::Document();
  doc.Parse(data.c_str());
  return doc;
}
#endif
