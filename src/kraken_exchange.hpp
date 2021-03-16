#pragma once
#include "base_types.hpp"
#include "types.hpp"
#include "macros.hpp"

namespace kraken_exchange {
let ws_uri = "wss://ws.kraken.com";
let api_url = "https://api.kraken.com";
let assets_path = "/0/public/AssetPairs";
def create_tick_unsub_request() -> str;
def create_tick_sub_request(const vec<str>& pairs) -> str;
def get_pairs_list() -> vec<str>;
def parse_event(String msg_data) -> optional<pair_price_update>;
}  // namespace kraken_exchange
#include "unmacros.hpp"
