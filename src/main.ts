#include "common.hpp"

#pragma region usings
using std::exception;
using std::runtime_error;
using std::string;
using std::variant;
using std::vector;

using rapidjson::Document;
using rapidjson::Value;

using web::http::methods;
using web::http::status_codes;
using web::http::client::http_client;
using web::http::client::http_client_config;

using fmt::format;
using fmt::print;
#pragma endregion

let get_pairs_list = [](
  const string &api_url,
  const string &assets_path
) -> variant<vector<string>, exception> {
  // get tradeable assets
  http_client_config config;
  config.set_validate_certificates(false);
  let response = http_client(api_url, config).request(methods::GET, assets_path).get();

  // if not OK then return an  error
  if (response.status_code() != status_codes::OK)
    return runtime_error("Returned " + to_string(response.status_code()));

  // extract the text from the response
  let response_text = response.extract_string().get();

  // attempt to parse the doc
  mut json_doc = Document();
  json_doc.Parse(response_text.c_str());
  if (json_doc.HasParseError() || !json_doc.HasMember("result"))
    return runtime_error(format("Failed to parse returned document: {}", response_text.c_str()));

  // extract wsname from the doc
  vector<string> pair_list;
  for (let &pair : json_doc["result"].GetObject())
    if (pair.value.HasMember("wsname"))
      pair_list.push_back(pair.value["wsname"].GetString());

  return pair_list;
};

fn main(i16 argc, str argv[])->i16 {
  let pairResults = get_pairs_list(API_URL, ASSETS_PATH);
  if (is_a<exception>(pairResults)) {
    print("Failed to get instruments: {}\n", get_as<exception>(pairResults).what());
    return -1;
  }
  return 0;
}
