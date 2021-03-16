#pragma once
#include <cpprest/ws_client.h>
#include <memory>
#include "../base_types.hpp"

using WebSocketIncomingMessage = const web::websockets::client::websocket_incoming_message&;
using std::make_shared;

namespace deps {
using web::websockets::client::websocket_callback_client, web::websockets::client::websocket_outgoing_message;

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
}

using WebSocket = std::shared_ptr<deps::websocket>;

inline auto make_websocket(String address) -> WebSocket {
  return make_shared<deps::websocket>(address);
}

