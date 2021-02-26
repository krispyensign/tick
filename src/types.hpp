#ifndef types_hpp
#define types_hpp
#include <msgpack.hpp>

#include "base_types.hpp"

#pragma region data structures
enum exchange_name {
  KRAKEN,
};

struct service_config {
  str zbind;
  str ws_uri;
  str api_url;
  str assets_path;
};

struct pair_price_update {
  str trade_name;
  double ask;
  double bid;
  MSGPACK_DEFINE(trade_name, ask, bid);
};
#pragma endregion

#endif
