#pragma once
#include <msgpack.hpp>
#include "../common/base_types.hpp"

namespace deps {
using std::stringstream, msgpack::pack;
struct Encoder {
  Encoder() = delete;
  template<typename T>
  static auto encode(const T& enc) -> str {
    auto buf = std::stringstream();
    msgpack::pack(buf, enc);
    return buf.str();
  }
};
}
using Encoder = deps::Encoder;
