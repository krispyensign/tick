#ifndef kraken_hpp
#define kraken_hpp

#include "types.hpp"

namespace kraken {
    auto get_pairs_list(const str& api_url, const str& assets_path) -> vec<str>;
    auto parse_event(const str& msg_data) -> var<pair_price_update, exception>;
    auto create_tick_sub_request(const vec<str>& pairs) -> tick_sub_req;
}

#endif
