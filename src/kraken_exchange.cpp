// clang-format off
#include "deps/ranges.hpp"
// clang-format on

#include "deps/formatter.hpp"
#include "deps/http.hpp"
#include "deps/json.hpp"
#include "kraken_exchange.hpp"

// clang-format off
#include "macros.hpp"
// clang=format on

namespace kraken_exchange {

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

def create_tick_sub_request(const vec<str>& pairs) -> str {
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
  if (json_doc.HasParseError() or not json_doc.HasMember("result")) {
    throw error("failed to parse returned document: " + response_text);
  }
  let obj = json_doc["result"].GetObject();

  // aggregate the wsnames of each pair to a vector of strings
  return obj | filter([](val pair) { return pair.value.HasMember("wsname"); })
         | transform([](val pair) { return pair.value["wsname"].GetString(); }) | to<vec<str>>;
}

def parse_tick(String msg_data) -> optional<pair_price_update> {
  // validate the event parsed and there were not errors on the message itself
  let msg = make_json(msg_data);
  if (msg.HasParseError()) {
    throw error("failed to parse: " + msg_data);
  }
  if (msg.IsObject() and msg.HasMember("errorMessage")) {
    throw error(msg["errorMessage"].GetString());
  }

  // validate it is a kraken publication type.  all kraken publications are arrays of size 4 then
  // cast it to an array
  if (not msg.IsArray() or msg.Size() != 4) {
    return null;
  }
  let publication = msg.GetArray();

  // validate the payload is a tick object
  let payload = publication[1].GetObject();
  let required_members = {"a", "b", "c", "v", "p", "t", "l", "h", "o"};
  if (not all_of(required_members,
                 [&payload](const auto& mem) { return payload.HasMember(mem); })) {
    return null;
  }

  // construct a neutral format for the price update event
  return pair_price_update{
    .trade_name = publication[3].GetString(),
    .ask = atof(payload["a"][0].GetString()),
    .bid = atof(payload["b"][0].GetString()),
  };
}
}  // namespace kraken_exchange
