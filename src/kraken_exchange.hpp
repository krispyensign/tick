#pragma once
#include "base_types.hpp"
#include "types.hpp"

#include "macros.hpp"

namespace kraken_exchange {
constexpr let ws_uri = "wss://ws.kraken.com";
constexpr let ws_auth_uri = "wss://ws-auth.kraken.com";
constexpr let api_url = "https://api.kraken.com";
constexpr let assets_path = "/0/public/AssetPairs";
constexpr let tick_members = {"a", "b", "c", "v", "p", "t", "l", "h", "o"};

def create_tick_unsub_request() -> str;
def create_tick_sub_request(Vector<str> pairs) -> str;
def get_pairs_list() -> vec<str>;
def parse_tick(String msg_data) -> optional<pair_price_update>;

struct add_order {
  str event = "addOrder";
  str token;
  optional<i32> reqid;
  str orderType;
  str type;
  str pair;
  f64 price;
  optional<f64> price2;
  str volume;
  optional<i32> leverage;
  optional<str> oflags;
  optional<i32> starttm;
  optional<i32> expiretm;
  optional<str> userref;
  optional<str> validate = "true";
  optional<str> close_ordertype;
  optional<f64> close_price;
  optional<f64> close_price2;
  optional<str> trading_agreement;
  [[nodiscard]] def serialize() const -> str;
  MSGPACK_DEFINE(event, token, reqid, orderType, type, pair, price, price2,
                 volume, leverage, oflags, starttm, expiretm, userref, validate,
                 close_ordertype, close_price, close_price2, trading_agreement)
};

struct add_order_status {
  str event;
  optional<int> reqid;
  str status;
  optional<str> txid;
  optional<str> descr;
  optional<str> errorMessage;
  MSGPACK_DEFINE(event, reqid, status, txid, descr, errorMessage)
};

struct cancel_order_status {
  str event;
  optional<long long> reqid;
  str status;
  optional<str> errorMessage;
  MSGPACK_DEFINE(event, reqid, status)
};

} // namespace kraken_exchange
#include "unmacros.hpp"
