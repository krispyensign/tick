#ifndef templates_hpp
#define templates_hpp

#include <algorithm>
#include <numeric>
#include <variant>
#include <vector>
using namespace std;

#include <spdlog/spdlog.h>

// helper type for the visitor #4
template <typename>
inline constexpr bool always_false_v = false;

template <typename... Ts>
struct exhaustive : Ts... {
  using Ts::operator()...;
  exhaustive(const Ts&... ts) : Ts{ts}... {};
};

template <typename T, typename... Funcs>
auto type_match(T tin, Funcs... funcs) {
  return std::visit(exhaustive{funcs...}, tin);
}

template <typename A>
auto wrap_error(A a) -> decltype(a()) {
  try {
    a();
  } catch (const exception& e) {
    spdlog::error(e.what());
    throw e;
  }
}

template <typename CONTAINER, typename NEW_CONTAINER, typename LAMBDA>
auto fold(CONTAINER container, NEW_CONTAINER new_container, LAMBDA lambda) -> NEW_CONTAINER {
  return std::accumulate(container.begin(), container.end(), new_container, lambda);
}

template <typename CONTAINER, typename LAMBDA>
auto maplist(CONTAINER&& container, LAMBDA&& lambda)
  -> std::vector<decltype(lambda(*container.begin()))> {
  std::vector<decltype(lambda(*container.begin()))> w;
  std::transform(container.begin(), container.end(), back_inserter(w), lambda);
  return w;
}

template <typename CONTAINER, typename LAMBDA>
auto filter(CONTAINER&& container, LAMBDA&& lambda) -> CONTAINER&& {
  CONTAINER&& w{};
  std::copy_if(container.begin(), container.end(), back_inserter(w), lambda);
  return w;
}

#endif
