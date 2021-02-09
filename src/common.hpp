#ifndef common_hpp
#define common_hpp

#pragma region includes
#include <algorithm>
#include <iterator>
#include <string>
#include <exception>
#include <stdexcept>
#include <variant>
#include <vector>
#include <algorithm>
#include <utility>

#include <cpprest/http_client.h>
#include <cpprest/http_msg.h>

#include <rapidjson/document.h>

#include <fmt/core.h>
#pragma endregion

#pragma region macros
#ifndef API_URL
#define API_URL "https://api.kraken.com"
#endif

#ifndef ASSETS_PATH
#define ASSETS_PATH "/0/public/AssetPairs"
#endif

#define let const auto
#define letmut auto
#define function auto
#pragma endregion

#pragma region types
typedef int i16;
typedef unsigned int u16;
typedef long i32;
typedef unsigned long u32;
typedef long long i64;
typedef unsigned long long u64;
typedef float f32;
typedef double f64;
typedef char* str;
#pragma endregion

#pragma region templates
template <typename CONTAINER, typename NEW_CONTAINER, typename LAMBDA>
function fold(CONTAINER container, NEW_CONTAINER new_container, LAMBDA lambda) {
  return accumulate(container.begin(), container.end(), new_container, lambda);
}

template<typename CONTAINER, typename LAMBDA>
function maplist(CONTAINER&& container, LAMBDA&& lambda) {
      std::vector<decltype(lambda(*container.begin()))> w;
      std::transform(container.begin(),container.end(),std::back_inserter(w), lambda);
      return w;
}

template <typename CONTAINER, typename LAMBDA>
function filter(CONTAINER&& container, LAMBDA&& lambda) {
    CONTAINER&& w{};
    std::copy_if(container.begin(), container.end(), std::back_inserter(w), lambda);
    return w;
}

template <typename T> function to_string(T t) -> std::string {
  return std::to_string(t);
};

template <typename T, typename U> function is_a(U u) -> bool {
  return std::holds_alternative<T>(u);
}

template <typename T, typename U> function get_as(U u) {
  return std::get<T>(u);
}
#pragma endregion

#endif
