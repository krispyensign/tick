#ifndef kraken_hpp
#define kraken_hpp

#include "types.hpp"

namespace kraken {
struct tick_sub_req {
  str event;
  vec<str> pairs;
  struct {
    str name;
  } subscription;
  auto serialize() -> str {
    return fmt::format(
      R"EOF(
      {{
        "event":{},
        "pairs":{},
        "subscription":{{
          "name": {}
        }}
      }}
      )EOF",
      event, pairs, subscription.name);
  }
};

const str tick_unsub_req = R"EOF(
{
  "event": "unsubscribe",
  "subscription": {
    "name": "*"
  }
}
)EOF";

auto get_pairs_list(const str& api_url, const str& assets_path) -> vec<str>;
auto parse_event(const str& msg_data) -> var<pair_price_update, exception>;
auto create_tick_sub_request(const vec<str>& pairs) -> str;
inline auto create_tick_unsub_request() -> str {
  return tick_unsub_req;
};

}  // namespace kraken

#endif
