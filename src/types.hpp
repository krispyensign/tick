#pragma once
#include <msgpack.hpp>

#include "base_types.hpp"

struct pair_price_update {
  str trade_name;
  double ask;
  double bid;
  MSGPACK_DEFINE(trade_name, ask, bid)
};

struct exchange_interface {
  std::function<str(const vec<str>&)> create_tick_sub_request;
  std::function<vec<str>(void)> get_pairs_list;
  std::function<str(void)> create_tick_unsub_request;
  std::function<std::optional<pair_price_update>(String)> parse_event;
  const str ws_uri;
};

#define EXCHANGE_INF_CASE(x)                   \
  case exchange_name::x:                       \
    return {                                   \
      x##_exchange::create_tick_sub_request,   \
      x##_exchange::get_pairs_list,            \
      x##_exchange::create_tick_unsub_request, \
      x##_exchange::parse_event,               \
      x##_exchange::ws_uri,                    \
    };

struct exchange_name {
  enum inner {
    kraken,
  } inner;

  static auto as_enum(String lookup) -> std::optional<exchange_name> {
#define make_exchange(x) \
  if (lookup == #x) return exchange_name{exchange_name::x}
    make_exchange(kraken);
    return null;
  };
};
