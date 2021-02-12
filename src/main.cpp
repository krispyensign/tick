#include "common.hpp"
#include "cpprest/http_client.h"

#pragma region usings
using except = std::exception;
using std::runtime_error;
using str = std::string;
using std::variant;
using std::vector;

#define vec vector
#define var variant

using rapidjson::Document;
using rapidjson::Value;

auto &GET = web::http::methods::GET;
auto &OK =  web::http::status_codes::OK;
using web::http::client::http_client;
using web::http::client::http_client_config;

using fmt::format;
using fmt::print;
#pragma endregion

function get_pairs_list(
  const str& api_url,
  const str& assets_path
) -> var<vec<str>, except> begin
  // disable ssl configs for now
  mut config = http_client_config();
  config.set_validate_certificates(false);

  // make the call and get a response back
  let response = http_client(api_url, config).request(GET, assets_path).get();

  // if not OK then return an error
  if (response.status_code() not_eq OK) then
    return runtime_error("Returned " + to_string(response.status_code()));
  end

  // extract the text from the response
  let response_text = response.extract_string().get();

  // attempt to parse the doc
  mut json_doc = Document();
  json_doc.Parse(response_text.c_str());
  if (json_doc.HasParseError() or not json_doc.HasMember("result")) then
    return runtime_error(format("Failed to parse returned document: {}", response_text.c_str()));
  end

  // extract wsname from the doc
  let obj = json_doc["result"].GetObject();
  mut pair_list = vec<str>();
  for (let &pair in obj) begin
    if (pair.value.HasMember("wsname")) then
      pair_list.push_back(pair.value["wsname"].GetString());
    end
  end

  return pair_list;
end;

function main(i16 argc, c_str argv[]) -> i16 begin
  let pairResults = get_pairs_list(API_URL, ASSETS_PATH);
  if (is_a<except>(pairResults)) then
    print("Failed to get instruments: {}\n", get_as<except>(pairResults).what());
    return -1;
  end
  return 0;
end
