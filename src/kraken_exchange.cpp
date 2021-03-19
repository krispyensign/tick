#include "deps/ranges.hpp"

#include "kraken_exchange.hpp"

#include "deps/formatter.hpp"
#include "deps/http.hpp"
#include "deps/json.hpp"

#include "macros.hpp"

namespace kraken_exchange {

def add_order::serialize() const -> str {
  // populate the document
  mutant doc = Document();
  doc.SetObject();
  docset(doc, event, token, reqid, orderType, type, pair, price, volume);
  docsetopt(doc, reqid, price2, leverage, oflags, starttm, expiretm, userref,
            validate, trading_agreement);
  docsetoptspec(doc, close_ordertype, close[ordertype]);
  docsetoptspec(doc, close_price, close[price]);
  docsetoptspec(doc, close_price2, close[price2]);
  return ::serialize(doc);
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
  return obj
    | filter([](let &pair) { return pair.value.HasMember("wsname"); })
    | transform([](let &pair) { return pair.value["wsname"].GetString(); })
    | to<vec<str>>;
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
