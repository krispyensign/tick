#include "common.hpp"

#pragma region usings
using std::runtime_error;
using std::exception;
using std::string;
using std::variant;
using std::vector;

using rapidjson::Document;
using rapidjson::Value;

using web::http::methods;
using web::http::status_codes;
using web::http::client::http_client;
using web::http::client::http_client_config;

using fmt::print;
#pragma endregion


let get_pairs_list = [](
  string api_url,
  string assets_path
) -> variant<vector<string>, exception> {
  // get tradeable assets
  http_client_config config;
  config.set_validate_certificates(false);
  try {
    let response = http_client(api_url, config).request(methods::GET, assets_path).get();

    // if not OK then return an  error
    if (response.status_code() != status_codes::OK)
      return runtime_error("Returned " + to_string(response.status_code()));

    // create a doc object and parse the response
    return fold(
      Document().Parse((response.extract_string().get()).c_str())["result"].GetObject(),
      vector<string>(),
      [](auto pairs, auto &ele) {
        if (ele.value.HasMember("wsname"))
          pairs.push_back(ele.value["wsname"].GetString());
        return pairs;
      });
  } catch(const exception &e) {
    return e;
  }
};

function main(i16 argc, str argv[])->i16 {
  let pairResults = get_pairs_list(API_URL, ASSETS_PATH);
  if (is_a<exception>(pairResults)) {
    let err = get_as<exception>(pairResults);
    print(err.what());
    print("Failed to get instruments\n");
    return -1;
  }
  return 0;
}
