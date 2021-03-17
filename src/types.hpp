#pragma once
#include <msgpack.hpp>

#include "base_types.hpp"

#include "macros.hpp"

struct pair_price_update {
  str trade_name;
  double ask;
  double bid;
  MSGPACK_DEFINE(trade_name, ask, bid)
};

struct exchange_interface {
  function<str(Vector<str>)> create_tick_sub_request;
  function<vec<str>(void)> get_pairs_list;
  function<str(void)> create_tick_unsub_request;
  function<optional<pair_price_update>(String)> parse_event;
  const str ws_uri;
};

#define EXCHANGE_INF_CASE(x)                   \
  case exchange_name::x:                       \
    return {                                   \
      x##_exchange::create_tick_sub_request,   \
      x##_exchange::get_pairs_list,            \
      x##_exchange::create_tick_unsub_request, \
      x##_exchange::parse_tick,                \
      x##_exchange::ws_uri,                    \
    };

#define MAKE_EXCHANGE(x)                   \
  if (lookup == #x) return exchange_name { \
      exchange_name::x                     \
    }

struct exchange_name {
  enum inner {
    kraken,
  } inner;

  static def as_enum(String lookup) -> optional<exchange_name> {
    MAKE_EXCHANGE(kraken);
    return null;
  };
};
#include "unmacros.hpp"
