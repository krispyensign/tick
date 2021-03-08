#ifndef kraken_hpp
#define kraken_hpp
#include <range/v3/all.hpp>

#include <rapidjson/document.h>
#include <rapidjson/rapidjson.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include "types.hpp"

using std::optional;

#define let const auto

namespace kraken_exchange {

using String = const str&;
using JsonValue = const json::Value&;

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
  return fmt::format(json_doc, fmt::join(pairs, "\",\""));
};

let parse_json = [](String response_text) -> vec<str> {
  // provision and parse doc
  auto json_doc = json::Document();
  json_doc.Parse(response_text.c_str());

  // validate it parsed correctly and has the field "result"
  if (json_doc.HasParseError() or not json_doc.HasMember("result")) {
    throw error("failed to parse returned document: " + response_text);
  }
  let obj = json_doc["result"].GetObject();

  // aggregate the wsnames of each pair to a vector of strings
  return obj
    | ranges::views::filter([](const auto& pair) { return pair.value.HasMember("wsname"); })
    | ranges::views::transform([](const auto& pair) { return pair.value["wsname"].GetString(); })
    | ranges::to<vec<str>>;
};

let get_pairs_list = [](String api_url, String assets_path) -> vec<str> {
  logger::info("getting kraken pairs list");
  // disable ssl configs for now
  auto config = rest::config();
  config.set_validate_certificates(false);

  // make the call and get a response back
  let response =
    rest::client(api_url, config).request(rest::methods::GET, assets_path).get();

  // if not OK then return an error
  if (response.status_code() != rest::status_codes::OK) {
    throw error("returned " + std::to_string(response.status_code()));
  }

  // extract and parse
  return parse_json(response.extract_string().get());
};

let is_ticker = [](JsonValue json) -> bool {
  // validate the payload has the following fields
  let required_members = {"a", "b", "c", "v", "p", "t", "l", "h", "o"};

  // check each member in the requried members list
  return ranges::all_of(required_members, [&json](const auto& mem) { return json.HasMember(mem); });
};

let parse_event = [](String msg_data) -> optional<pair_price_update> {
  auto msg = json::Document();
  msg.Parse(msg_data.c_str());

  // validate the event parsed and there were not errors on the message itself
  if (msg.HasParseError()) {
    throw error("failed to parse: " + msg_data);
  }

  // if message is {} object and is an error object then throw
  if (msg.IsObject() and msg.HasMember("errorMessage")) {
    throw error(msg["errorMessage"].GetString());
  }

  // validate it is a kraken publication type.  all kraken publications are arrays of size 4
  if (not msg.IsArray() or msg.Size() != 4) {
    return null;
  }

  // cast the message to a publication array
  let publication = msg.GetArray();

  // get the payload from the publication as an object
  let payload = publication[1].GetObject();

  // validate the payload is a tick object
  if (not is_ticker(payload)) {
    return null;
  }

  // construct a neutral format for the price update event
  return pair_price_update{
    .trade_name = publication[3].GetString(),
    .ask = atof(payload["a"][0].GetString()),
    .bid = atof(payload["b"][0].GetString()),
  };

};
}  // namespace kraken_exchange

#endif
