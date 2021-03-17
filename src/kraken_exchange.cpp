#include "deps/ranges.hpp"

#include "kraken_exchange.hpp"

#include "deps/formatter.hpp"
#include "deps/http.hpp"
#include "deps/json.hpp"

#include "macros.hpp"
#include "rapidjson/stringbuffer.h"

namespace kraken_exchange {

def serialize_add_order(add_order ao) -> str {
  mutant sb = StringBuffer();
  mutant w = PrettyWriter(sb);
  w.StartObject();

  S_STRING(w, ao.event)
  S_STRING(w, ao.token)
  S_OPTINT(w, ao.reqid)
  S_STRING(w, ao.orderType)
  S_STRING(w, ao.type)
  S_STRING(w, ao.pair)
  S_DOUBLE(w, ao.price)
  S_OPTDOU(w, ao.price2)
  S_STRING(w, ao.volume)
  S_OPTINT(w, ao.leverage)
  S_OPTSTR(w, ao.oflags)
  S_OPTINT(w, ao.starttm)
  S_OPTINT(w, ao.expiretm)
  S_OPTSTR(w, ao.userref)
  S_OPTSTR(w, ao.validate)
  S_OPTSTR2(w, ao.close_ordertype, "close[ordertype]")
  S_OPTDOU2(w, ao.close_price, "close[price]")
  S_OPTDOU2(w, ao.close_price2, "close[price2]")

  w.EndObject();
  return sb.GetString();
}

def create_tick_unsub_request() -> str {
  return R"EOF(
    {
      "event": "unsubscribe",
      "subscription": {
        "name": "ticker"
      }
    }
  )EOF";
}

def create_tick_sub_request(Vector<str> pairs) -> str {
  return format(R"EOF(
    {{
      "event": "subscribe",
      "pair": ["{}"],
      "subscription": {{
        "name": "ticker"
      }}
    }}
  )EOF",
                join(pairs, "\",\""));
}

def get_pairs_list() -> vec<str> {
  // make the call and get a response back
  let response = http_client(api_url).request(methods::GET, assets_path).get();

  // if not OK then return an error
  if (response.status_code() != status_codes::OK) {
    throw error(format("returned: {}", response.status_code()));
  }

  // extract and parse
  let response_text = response.extract_string().get();

  // parse then validate it has the field "result"
  let json_doc = make_json(response_text);
  if (not json_doc.HasMember("result")) {
    throw error("failed to parse returned document: " + response_text);
  }
  let obj = json_doc["result"].GetObject();

  // aggregate the wsnames of each pair to a vector of strings
  return obj |
         filter([](let &pair) { return pair.value.HasMember("wsname"); }) |
         transform([](let &pair) { return pair.value["wsname"].GetString(); }) |
         to<vec<str>>;
}

def parse_tick(String msg_data) -> optional<pair_price_update> {
  // validate the event parsed and there were not errors on the message itself
  let msg = make_json(msg_data);
  if (msg.IsObject() and msg.HasMember("errorMessage")) {
    throw error(msg["errorMessage"].GetString());
  }

  // validate it is a kraken publication type.  all kraken publications are
  // arrays of size 4 then cast it to an array
  if (not msg.IsArray() or msg.Size() != 4) {
    return null;
  }
  let publication = msg.GetArray();

  // validate the payload is a tick object
  let payload = publication[1].GetObject();
  if (not all_of(tick_members,
                 [&payload](let &mem) { return payload.HasMember(mem); })) {
    return null;
  }

  // construct a neutral format for the price update event
  return pair_price_update{
      .trade_name = publication[3].GetString(),
      .ask = atof(payload["a"][0].GetString()),
      .bid = atof(payload["b"][0].GetString()),
  };
}

def parse_order_event(String msg_data)
    ->optional<var<add_order_status, cancel_order_status>> {
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
