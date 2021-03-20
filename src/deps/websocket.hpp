#pragma once
#include <cpprest/ws_client.h>
#include <memory>
#include "../common/base_types.hpp"

#include "../common/macros.hpp"

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

  def send(String msg) -> void {
    mutant out_msg = websocket_outgoing_message();
    out_msg.set_utf8_message(msg);
    client.send(out_msg).get();
  }
  template <typename T>
  def set_message_handler(T t) -> void {
    client.set_message_handler(t);
  }
};
}

using WebSocket = std::shared_ptr<deps::websocket>;

inline def make_websocket(String address) -> WebSocket {
  return make_shared<deps::websocket>(address);
}

#include "../common/unmacros.hpp"
