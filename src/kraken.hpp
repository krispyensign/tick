#ifndef kraken
#define kraken

#include "cpprest/http_msg.h"
#include "macros.hpp"
#include "templates.hpp"
#include "types.hpp"

let api_url = "https://api.kraken.com";
let assets_path = "/0/public/AssetPairs";

let create_tick_sub_request = $(vec<str>& pairs) -> tick_sub_req {
  return {
    .event = "subscribe",
    .pairs = pairs,
    .subscription =
      {
        .name = "ticker",
      },
  };
};

let parse_json = $(str& response_text) -> vec<str> {
  mut json_doc = json::Document();
  json_doc.Parse(response_text.c_str());

  // validate it parsed correctly and has the field "result"
  if (json_doc.HasParseError() or not json_doc.HasMember("result"))
    throw error(fmt::format("Failed to parse returned document: {}", response_text.c_str()));
  let obj = json_doc["result"].GetObject();

  // aggregate the wsnames of each pair to a vector of strings
  mut pair_list = vec<str>();
  for (let& pair in obj)
    if (pair.value.HasMember("wsname")) pair_list.push_back(pair.value["wsname"].GetString());

  // return the vector
  return pair_list;
};

let get_pairs_list = $(str& api_url, str& assets_path) -> pplx::task<vec<str>> {
  // disable ssl configs for now
  mut config = rest::client_config();
  config.set_validate_certificates(false);

  // make the call and get a response back
  return rest::client(api_url, config)
    .request(rest::methods::GET, assets_path)
    .then($$(rest::response& response) {
      // if not OK then return an error
      if (response.status_code() != rest::status_codes::OK) {
        throw error("Returned " + to_string(response.status_code()));
      }

      // else extract as a string
      return response.extract_string();
    })
    .then($$(str& response_text) { return parse_json(response_text); });
};

let is_ticker = $(json::Value& json) -> bool {
  // validate the payload has the following fields
  let required_members = {"a", "b", "c", "v", "p", "t", "l", "h", "o"};

  // check each member in the requried members list
  for (let& memb in required_members)
    if (not json.HasMember(memb)) return false;

  return true;
};

let parse_event = $(str& msg_data) -> result<pair_price_update> {
  mut msg = json::Document();
  msg.Parse(msg_data.c_str());

  // validate the event parsed and there were not errors on the message itself
  if (msg.HasParseError() or msg.HasMember("errorMessage"))
    return error(msg["errorMessage"].GetString());

  // validate it is a kraken publication type.  all kraken publications are arrays of size 4
  if (msg.Size() != 4) return error("Not a publication");

  // get the payload of the publication
  let payload = msg[1].GetObject();

  // validate the payload is a tick object
  if (not is_ticker(payload)) return error("Payload is not a tick");

  // construct a neutral format for the price update event
  return pair_price_update{
    .trade_name = msg[3].GetString(),
    .ask = payload["a"][0].GetDouble(),
    .bid = payload["b"][0].GetDouble(),
  };
};
#endif
