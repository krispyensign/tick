#pragma once
#include <msgpack.hpp>
#include "../base_types.hpp"

namespace deps {
using std::stringstream, msgpack::pack;
struct Encoder {
  Encoder() = delete;
  static auto encode(String enc) -> str {
    auto buf = std::stringstream();
    msgpack::pack(buf, enc);
    return buf.str();
  }
};
}
using Encoder = deps::Encoder;
