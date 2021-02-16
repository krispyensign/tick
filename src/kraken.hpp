#ifndef kraken_hpp
#define kraken_hpp

#include "types.hpp"
#include <stdexcept>

let api_url = "https://api.kraken.com";
let assets_path = "/0/public/AssetPairs";

let create_tick_sub_request = $(vec<str>& pairs) -> tick_sub_req {
  return {
    .event = "subscribe",
    .pairs = pairs,
    .subscription = {
      .name = "ticker",
    },
  };
};

let get_pairs_list = $(str& api_url, str& assets_path) -> var<vec<str>, exception> {
  // disable ssl configs for now
  mut config = rest::client_config();
  config.set_validate_certificates(false);

  // make the call and get a response back
  let response = rest::client(api_url, config).request(rest::methods::GET, assets_path).get();

  // if not OK then return an error
  if (response.status_code() != rest::status_codes::OK)
    return error("Returned " + to_string(response.status_code()));

  // extract the text from the response
  let response_text = response.extract_string().get();

  // attempt to parse the doc
  mut json_doc = json::Document();
  json_doc.Parse(response_text.c_str());
  if (json_doc.HasParseError() || !json_doc.HasMember("result"))
    return error(fmt::format("Failed to parse returned document: {}", response_text.c_str()));

  // extract wsnames from the doc
  let obj = json_doc["result"].GetObject();
  mut pair_list = vec<str>();
  for (let &pair : obj)
    if (pair.value.HasMember("wsname"))
      pair_list.push_back(pair.value["wsname"].GetString());

  return pair_list;
};

let is_ticker = $(json::Value& json) -> bool {
  // validate the payload has the following fields
  let required_members = {"a", "b", "c", "v", "p", "t", "l", "h", "o"};

  // check each member in the requried members list
  for (let &memb : required_members) if (!json.HasMember(memb)) return false;

  return true;
};

let parse_event = $(str& msg_data) -> var<pair_price_update, exception> {
  mut msg = json::Document();
  msg.Parse(msg_data.c_str());

  // validate the event parsed and there were not errors on the message itself
  if (msg.HasParseError() || msg.HasMember("errorMessage")) return error(msg["errorMessage"].GetString());

  // validate it is a kraken publication type.  all kraken publications are arrays of size 4
  if (msg.Size() != 4) return error("Not a publication");

  // get the payload of the publication
  let payload = msg[1].GetObject();

  // validate the payload is a tick object
  if (!is_ticker(payload)) return error("Payload is not a tick");

  // construct a neutral format for the price update event
  return pair_price_update{
    .trade_name = msg[3].GetString(),
    .ask = payload["a"][0].GetDouble(),
    .bid = payload["b"][0].GetDouble(),
  };
};
#endif
