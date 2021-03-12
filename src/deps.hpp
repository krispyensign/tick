#pragma once
#define FMT_HEADER_ONLY 1
#define BACKWARD_HAS_LIBUNWIND 1
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
#include <memory>
#include <msgpack.hpp>
#include <zmq.hpp>

#include "base_types.hpp"

using Context = std::shared_ptr<zmq::context_t>;
using WebSocketIncomingMessage = const web::websockets::client::websocket_incoming_message&;

namespace deps {
using std::shared_ptr, zmq::context_t, zmq::socket_t, zmq::socket_type, zmq::send_flags,
  std::make_shared, zmq::message_t, std::stringstream, msgpack::pack,
  web::websockets::client::websocket_callback_client, rapidjson::Document,
  web::websockets::client::websocket_outgoing_message;
struct Encoder {
  Encoder() = delete;
  static auto encode(String enc) -> str {
    auto buf = std::stringstream();
    msgpack::pack(buf, enc);
    return buf.str();
  }
};

struct publisher {
 private:
  socket_t sock;

 public:
  publisher() = delete;
  publisher(Context ctx, String bind_addr) {
    sock = socket_t(*ctx, socket_type::pub);
    sock.bind(bind_addr);
  }
  ~publisher() { sock.close(); }
  auto send(String msg) -> void { sock.send(message_t(msg), send_flags::none); };
};

struct websocket {
 private:
  websocket_callback_client client;

 public:
  websocket() = delete;
  websocket(String address) { client.connect(address).get(); }
  ~websocket() { client.close(); }

  auto send(String msg) -> void {
    auto out_msg = websocket_outgoing_message();
    out_msg.set_utf8_message(msg);
    client.send(out_msg).get();
  }
  template <typename T>
  auto set_message_handler(T t) -> void {
    client.set_message_handler(t);
  }
};
}  // namespace deps

using Publisher = std::shared_ptr<deps::publisher>;
using WebSocket = std::shared_ptr<deps::websocket>;
using Encoder = deps::Encoder;

inline auto make_publisher(Context ctx, String bind_addr) -> Publisher {
  return make_shared<deps::publisher>(ctx, bind_addr);
}

inline auto make_context(int threads) -> Context {
  return std::make_shared<zmq::context_t>(threads);
}
inline auto make_websocket(String address) -> WebSocket {
  return make_shared<deps::websocket>(address);
}

inline auto make_json(String data) -> rapidjson::Document {
  auto doc = rapidjson::Document();
  doc.Parse(data.c_str());
  return doc;
}
