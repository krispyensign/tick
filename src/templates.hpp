#ifndef templates
#define templates
#pragma region includes

#include <initializer_list>
#include <algorithm>
#include <string>
#include <variant>
#include <vector>
#include <numeric>
using namespace std;
#pragma endregion

#pragma region type-helpers
// helper type for the visitor #4
template<typename> inline constexpr bool always_false_v = false;

template<typename... Ts>
struct exhaustive : Ts... {
  using Ts::operator()...;
  exhaustive(const Ts&... ts) : Ts{ts}... {};
};

template<typename T>
struct type_switch {
  T data;
  type_switch(T tin) : data(tin) {}
  template<typename... Funcs>
  auto operator() (Funcs... funcs) {
    return type_match(data, funcs...);
  }
};

template<typename A, typename... Funcs>
auto type_match(A arg, Funcs... funcs) {
  return std::visit(exhaustive{funcs... }, arg);
}

template<typename T>
struct result : public variant<T, exception> {
  result(T tin) : variant<T, exception>(tin) {}
  result(exception ein) : variant<T, exception>(ein) {}
  result(variant<T, exception> data) : variant<T, exception>{data} {}

  auto unwrap() -> T {
    if(holds_alternative<exception>(this)) {
      throw get<exception>(this);
    }
    return get<T>(this);
  }
};
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
