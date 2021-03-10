#ifndef kraken_hpp
#define kraken_hpp
#include "deps.hpp"
#include "types.hpp"

using web::http::client::http_client, web::http::methods, web::http::status_codes,
  rapidjson::Document, rapidjson::Value, fmt::format, fmt::join, ranges::views::filter,
  ranges::views::transform, ranges::to, ranges::all_of;

namespace kraken_exchange {

let ws_uri = "wss://ws.kraken.com";
let api_url = "https://api.kraken.com";
let assets_path = "/0/public/AssetPairs";

let create_tick_unsub_request = []() -> str {
  return R"EOF(
    {
      "event": "unsubscribe",
      "subscription": {
        "name": "ticker"
      }
    }
  )EOF";
};

let create_tick_sub_request = [](const vec<str>& pairs) -> str {
  // allocate root dom
  let json_doc = R"EOF(
    {{
      "event": "subscribe",
      "pair": ["{}"],
      "subscription": {{
        "name": "ticker"
      }}
    }}
  )EOF";
  return format(json_doc, join(pairs, "\",\""));
};

let parse_json = [](const str& response_text) -> vec<str> {
  // provision and parse doc
  mutant json_doc = Document();
  json_doc.Parse(response_text.c_str());

  // validate it parsed correctly and has the field "result"
  if (json_doc.HasParseError() or not json_doc.HasMember("result"))
    throw error("failed to parse returned document: " + response_text);
  let obj = json_doc["result"].GetObject();

  // aggregate the wsnames of each pair to a vector of strings
  return obj | filter([](const auto& pair) { return pair.value.HasMember("wsname"); })
         | transform([](const auto& pair) { return pair.value["wsname"].GetString(); })
         | to<vec<str>>;
};

let get_pairs_list = []() -> vec<str> {
  // make the call and get a response back
  let response = http_client(api_url).request(methods::GET, assets_path).get();

  // if not OK then return an error
  if (response.status_code() != status_codes::OK)
    throw error(format("returned: {}", response.status_code()));

  // extract and parse
  return parse_json(response.extract_string().get());
};

let is_ticker = [](const Value& json_val) -> bool {
  // validate the payload has the following fields
  let required_members = {"a", "b", "c", "v", "p", "t", "l", "h", "o"};

  // check each member in the requried members list
  return all_of(required_members, [&json_val](const auto& mem) { return json_val.HasMember(mem); });
};

let parse_event = [](const str& msg_data) -> optional<pair_price_update> {
  mutant msg = Document();
  msg.Parse(msg_data.c_str());

  // validate the event parsed and there were not errors on the message itself
  if (msg.HasParseError()) throw error("failed to parse: " + msg_data);

  // if message is {} object and is an error object then throw
  if (msg.IsObject() and msg.HasMember("errorMessage"))
    throw error(msg["errorMessage"].GetString());

  // validate it is a kraken publication type.  all kraken publications are
  // arrays of size 4
  if (not msg.IsArray() or msg.Size() != 4) return null;

  // cast the message to a publication array
  let publication = msg.GetArray();

  // get the payload from the publication as an object
  let payload = publication[1].GetObject();

  // validate the payload is a tick object
  if (not is_ticker(payload)) return null;

  // construct a neutral format for the price update event
  return pair_price_update{
    .trade_name = publication[3].GetString(),
    .ask = atof(payload["a"][0].GetString()),
    .bid = atof(payload["b"][0].GetString()),
  };
};
}  // namespace kraken_exchange

#endif
