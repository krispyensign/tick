#ifndef templates
#define templates
#include "types.hpp"
#pragma region includes
#include <initializer_list>
#include <algorithm>
#include <string>
#include <variant>
#include <vector>
#include <numeric>

#pragma endregion
#pragma region type-helpers
// helper type for the visitor #4
template<typename> inline constexpr bool always_false_v = false;

template<typename... Ts>
struct exhaustive : Ts... {
  using Ts::operator()...;
  exhaustive(const Ts&... ts) : Ts{ts}... {};
};

template<typename A, typename... Funcs>
auto type_match(A arg, Funcs... funcs) {
  return std::visit(exhaustive{funcs... }, arg);
}

template<typename... Ts>
struct exhaustive2 {
  exhaustive2(std::initializer_list<std::variant<Ts...>> il) {

  }
};

// struct x##awesome

#pragma endregion

#pragma region higher order functions
template <typename CONTAINER, typename NEW_CONTAINER, typename LAMBDA>
auto fold(CONTAINER container, NEW_CONTAINER new_container, LAMBDA lambda) -> NEW_CONTAINER {
  return std::accumulate(container.begin(), container.end(), new_container, lambda);
}

template<typename CONTAINER, typename LAMBDA>
auto maplist(CONTAINER&& container, LAMBDA&& lambda) -> std::vector<decltype(lambda(*container.begin()))>{
      std::vector<decltype(lambda(*container.begin()))> w;
      std::transform(container.begin(),container.end(),back_inserter(w), lambda);
      return w;
}

template <typename CONTAINER, typename LAMBDA>
auto filter(CONTAINER&& container, LAMBDA&& lambda) -> CONTAINER&& {
    CONTAINER&& w{};
    std::copy_if(container.begin(), container.end(), back_inserter(w), lambda);
    return w;
}
#pragma endregion

#endif
