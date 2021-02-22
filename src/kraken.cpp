#include "templates.hpp"
#include "types.hpp"
#include "macros.hpp"

namespace kraken {
auto create_tick_sub_request(const vec<str>& pairs) -> tick_sub_req {
  return {
    .event = "subscribe",
    .pairs = pairs,
    .subscription = {
      .name = "ticker",
    },
  };
};

auto parse_json(const str& response_text) -> vec<str> {
  // provision and parse doc
  auto json_doc = json::Document();
  json_doc.Parse(response_text.c_str());

  // validate it parsed correctly and has the field "result"
  if (json_doc.HasParseError() or not json_doc.HasMember("result"))
    throw error(fmt::format("Failed to parse returned document: {}", response_text.c_str()));
  const auto obj = json_doc["result"].GetObject();

  // aggregate the wsnames of each pair to a vector of strings
  auto pair_list = vec<str>();
  for (const auto& pair : obj)
    if (pair.value.HasMember("wsname")) pair_list.push_back(pair.value["wsname"].GetString());

  // return the vector
  return pair_list;
};

auto get_pairs_list(const str& api_url, const str& assets_path) -> vec<str> {
  logger::info("Getting kraken pairs list");
  // disable ssl configs for now
  auto config = rest::config();
  config.set_validate_certificates(false);

  // make the call and get a response back
  auto response = rest::client(api_url, config)
    .request(rest::methods::GET, assets_path).get();

  // if not OK then return an error
  if (response.status_code() != rest::status_codes::OK) {
    throw error("Returned " + to_string(response.status_code()));
  }

  // extract and parse
  return parse_json(response.extract_string().get());
};

auto is_ticker(const json::Value& json) -> bool {
  // validate the payload has the following fields
  const auto required_members = {"a", "b", "c", "v", "p", "t", "l", "h", "o"};

  // check each member in the requried members list
  for (const auto& memb : required_members)
    if (not json.HasMember(memb)) return false;

  return true;
};

auto parse_event(const str& msg_data) -> var<pair_price_update, exception> {
  auto msg = json::Document();
  msg.Parse(msg_data.c_str());

  // validate the event parsed and there were not errors on the message itself
  if (msg.HasParseError() or msg.HasMember("errorMessage"))
    return error(msg["errorMessage"].GetString());

  // validate it is a kraken publication type.  all kraken publications are arrays of size 4
  if (msg.Size() != 4) return error("Not a publication");

  // get the payload of the publication
  const auto payload = msg[1].GetObject();

  // validate the payload is a tick object
  if (not is_ticker(payload)) return error("Payload is not a tick");

  // construct a neutral format for the price update event
  return pair_price_update{
    .trade_name = msg[3].GetString(),
    .ask = payload["a"][0].GetDouble(),
    .bid = payload["b"][0].GetDouble(),
  };
};
}
