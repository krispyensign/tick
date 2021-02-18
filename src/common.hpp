#ifndef common
#define common

#include <initializer_list>
#pragma region includes
#include <algorithm>
#include <string>
#include <variant>
#include <vector>
#include <numeric>
#pragma endregion

#pragma region templates
// helper type for the visitor #4
template<class... Ts>
struct exhaustive : Ts... {
  using Ts::operator()...;
  exhaustive(const Ts&... ts) : Ts{ts}... {};
};

template<typename A, typename... Ts>
auto type_match(A arg, exhaustive<Ts...> matcher) {
  return std::visit(matcher, arg);
}

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


#pragma region macros

#define VA_NUM_ARGS(...) VA_NUM_ARGS_IMPL(__VA_ARGS__, 5,4,3,2,1)
#define VA_NUM_ARGS_IMPL(_1,_2,_3,_4,_5,N,...) N
#define macro_dispatcher(func, ...) macro_dispatcher_(func, VA_NUM_ARGS(__VA_ARGS__))
#define macro_dispatcher_(func, nargs) macro_dispatcher__(func, nargs)
#define macro_dispatcher__(func, nargs) func ## nargs
#define $(...) macro_dispatcher($, __VA_ARGS__)(__VA_ARGS__)
#define $1(a) [](const a)
#define $2(a, b) [](const a, const b)
#define $3(a, b, c) [](const a, const b, const c)
#define $$(...) macro_dispatcher($$, __VA_ARGS__)(__VA_ARGS__)
#define $$1(a) [=](const a)
#define $$2(a, b) [=](const a, const b)
#define $$3(a, b, c) [=](const a, const b, const c)

#define type_switch(...) macro_dispatcher(type_switch, __VA_ARGS__)(__VA_ARGS__)
#define type_switch2(a, b) type_match(a, exhaustive{[=]b});
#define type_switch3(a, b, c) type_match(a, exhaustive{[=]b, [=]c});
#define type_switch4(a, b, c, d) type_match(a, exhaustive{[=]b, [=]c, [=]d});
#define _ auto xyzzy


#define PP_COMMASEQ_N()                                    \
          1,  1,  1,  1,                                   \
          1,  1,  1,  1,  1,  1,  1,  1,  1,  1,           \
          1,  1,  1,  1,  1,  1,  1,  1,  1,  1,           \
          1,  1,  1,  1,  1,  1,  1,  1,  1,  1,           \
          1,  1,  1,  1,  1,  1,  1,  1,  1,  1,           \
          1,  1,  1,  1,  1,  1,  1,  1,  1,  1,           \
          1,  1,  1,  1,  1,  1,  1,  1,  0,  0

#define PP_COMMA(...)    ,

#define PP_HASCOMMA(...)                                   \
          PP_NARG_(__VA_ARGS__, PP_COMMASEQ_N())

#define PP_NARG(...)                                       \
          PP_NARG_HELPER1(                                 \
              PP_HASCOMMA(__VA_ARGS__),                    \
              PP_HASCOMMA(PP_COMMA __VA_ARGS__ ()),        \
              PP_NARG_(__VA_ARGS__, PP_RSEQ_N()))

#define PP_NARG_HELPER1(a, b, N)    PP_NARG_HELPER2(a, b, N)
#define PP_NARG_HELPER2(a, b, N)    PP_NARG_HELPER3_ ## a ## b(N)
#define PP_NARG_HELPER3_01(N)    0
#define PP_NARG_HELPER3_00(N)    1
#define PP_NARG_HELPER3_11(N)    N
#pragma endregion

#endif
