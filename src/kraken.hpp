#ifndef kraken_hpp
#define kraken_hpp
#include <rapidjson/document.h>
#include <rapidjson/rapidjson.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include "ns_helper.hpp"
#include "types.hpp"

using std::optional;

namespace kraken_exchange {

auto get_pairs_list(const str& api_url, const str& assets_path) -> vec<str>;
auto parse_event(const str& msg_data) -> optional<pair_price_update>;
auto create_tick_sub_request(const vec<str>& pairs) -> str;
auto create_tick_unsub_request() -> str;

}  // namespace kraken_exchange

#endif
