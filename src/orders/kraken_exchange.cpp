#include "../deps/ranges.hpp"

#include "kraken_exchange.hpp"

#include "../deps/formatter.hpp"
#include "../deps/http.hpp"
#include "../deps/json.hpp"
#include <tuple>

#include "../common/macros.hpp"

namespace kraken_exchange {
using std::tuple;

def add_order::serialize() const -> str {
  // populate the document
  return json::serialize(
      to_tuplestr(event, token, reqid, orderType, type, pair, price, volume,
                  reqid, price2, leverage, oflags, starttm, expiretm, userref,
                  validate, trading_agreement),
      tuple{"close[ordertype]", close_ordertype},
      tuple{"close[price]", close_price}, tuple{"close[price2]", close_price2});
}


def parse_order_event(String msg_data)
    -> optional<var<add_order_status, cancel_order_status>> {
  // validate msg and if the msg is not an object and is missing either the
  // reqid id or event type then it is not an event and return empty
  let msg = make_json(msg_data);
  if (not(msg.IsObject() and msg.HasMember("reqid") and
          msg.HasMember("event"))) {
    return null;
  }

  let event = str(msg["event"].GetString());
  if (event == "addOrderStatus") {
    return add_order_status{
        .event = event,
        .reqid =
            msg.HasMember("reqid") ? optional(msg["reqid"].GetInt64()) : null,
        .status = msg["status"].GetString(),
        .txid =
            msg.HasMember("txid") ? optional(msg["txid"].GetString()) : null,
        .descr =
            msg.HasMember("descr") ? optional(msg["descr"].GetString()) : null,
        .errorMessage = msg.HasMember("errorMessage")
                            ? optional(msg["errorMessage"].GetString())
                            : null,
    };
  }

  if (event == "cancelOrderStatus") {
    return cancel_order_status{
        .event = event,
        .reqid =
            msg.HasMember("reqid") ? optional(msg["reqid"].GetInt64()) : null,
        .status = msg["status"].GetString(),
        .errorMessage = msg.HasMember("errorMessage")
                            ? optional(msg["errorMessage"].GetString())
                            : null,
    };
  }

  return null;
}

} // namespace kraken_exchange
