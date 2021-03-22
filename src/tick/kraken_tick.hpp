#pragma once
#include "tick_types.hpp"
#include "../common/base_types.hpp"
#include "../common/macros.hpp"

namespace kraken_exchange {
    constexpr let ws_uri = "wss://ws.kraken.com";

    def create_tick_unsub_request() -> str;
    def create_tick_sub_request(Vector<str> pairs) -> str;
    def get_pairs_list() -> vec<str>;
    def parse_tick(String msg_data) -> optional<pair_price_update>;

}
#include "../common/unmacros.hpp"
