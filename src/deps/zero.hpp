#pragma once
#include <zmq.hpp>
#include "../base_types.hpp"

using Context = std::shared_ptr<zmq::context_t>;
namespace deps {
using zmq::context_t, zmq::socket_t, zmq::socket_type, zmq::send_flags, zmq::message_t;

struct publisher {
 private:
  socket_t sock;

 public:
  publisher() = delete;
  publisher(const Context& ctx, String bind_addr) {
    sock = socket_t(*ctx, socket_type::pub);
    sock.bind(bind_addr);
  }
  ~publisher() { sock.close(); }
  auto send(String msg) -> void { sock.send(message_t(msg), send_flags::none); };
};

struct pull {
 private:
  socket_t sock;

 public:
  pull() = delete;
  pull(const Context& ctx, String bind_addr) {
    sock = socket_t(*ctx, socket_type::pull);
    sock.bind(bind_addr);
  }
  ~pull() { sock.close(); }
  auto recv() -> optional<str> {
    auto msg = message_t();
    auto res = sock.recv(msg, zmq::recv_flags::none);
    if (res != null) {
      return msg.str();
    }
    return null;
  }
};
}

using Publisher = std::shared_ptr<deps::publisher>;
using Pull = std::shared_ptr<deps::pull>;

inline auto make_publisher(Context ctx, String bind_addr) -> Publisher {
  return make_shared<deps::publisher>(ctx, bind_addr);
}

inline auto make_pull(Context ctx, String bind_addr) -> Pull {
  return make_shared<deps::pull>(ctx, bind_addr);
}

inline auto make_context(int threads) -> Context {
  return std::make_shared<zmq::context_t>(threads);
}
