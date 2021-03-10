#ifndef types_hpp
#define types_hpp
#include <msgpack.hpp>
#include "base_types.hpp"

using std::function;
using std::optional;

#define let const auto
#define mutant auto
#define def auto

#define make_exchange(x)                   \
  if (lookup == #x) return exchange_name { \
      exchange_name::x                     \
    }

struct exchange_name {
  enum inner {
    kraken,
  } inner;

  static def as_enum(const str& lookup)->optional<exchange_name> {
    make_exchange(kraken);
    return null;
  };
};

struct pair_price_update {
  str trade_name;
  double ask;
  double bid;
  MSGPACK_DEFINE(trade_name, ask, bid)
};

struct exchange_interface {
  function<str(const vec<str>&)> create_tick_sub_request;
  function<vec<str>(void)> get_pairs_list;
  function<str(void)> create_tick_unsub_request;
  function<optional<pair_price_update>(const str&)> parse_event;
  const str ws_uri;
};

#endif
