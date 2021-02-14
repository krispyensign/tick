#ifndef common_hpp
#define common_hpp

#pragma region includes
#include <algorithm>
#include <string>
#include <variant>
#include <vector>
#include <utility>
#pragma endregion

#pragma region macros
#ifndef API_URL
#define API_URL "https://api.kraken.com"
#endif

#ifndef ASSETS_PATH
#define ASSETS_PATH "/0/public/AssetPairs"
#endif

#define let const auto
#define mut auto
#define def auto

#define VA_NUM_ARGS(...) VA_NUM_ARGS_IMPL(__VA_ARGS__, 5,4,3,2,1)
#define VA_NUM_ARGS_IMPL(_1,_2,_3,_4,_5,N,...) N
#define macro_dispatcher(func, ...) macro_dispatcher_(func, VA_NUM_ARGS(__VA_ARGS__))
#define macro_dispatcher_(func, nargs) macro_dispatcher__(func, nargs)
#define macro_dispatcher__(func, nargs) func ## nargs
#define λ(...) macro_dispatcher(λ, __VA_ARGS__)(__VA_ARGS__)
#define λ1(a) [](auto a)
#define λ2(a, b) [](auto a, auto b)
#define $(...) macro_dispatcher($, __VA_ARGS__)(__VA_ARGS__)
#define $1(a) [](const a)
#define $2(a, b) [](const a, const b)

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

#pragma region types
typedef int i16;
typedef unsigned int u16;
typedef long i32;
typedef unsigned long u32;
typedef long long i64;
typedef unsigned long long u64;
typedef float f32;
typedef double f64;
typedef char* c_str;
#pragma endregion

#pragma region templates
template <typename CONTAINER, typename NEW_CONTAINER, typename LAMBDA>
def fold(CONTAINER container, NEW_CONTAINER new_container, LAMBDA lambda) {
  return accumulate(container.begin(), container.end(), new_container, lambda);
}

template<typename CONTAINER, typename LAMBDA>
def maplist(CONTAINER&& container, LAMBDA&& lambda) {
      std::vector<decltype(lambda(*container.begin()))> w;
      std::transform(container.begin(),container.end(),std::back_inserter(w), lambda);
      return w;
}

template <typename CONTAINER, typename LAMBDA>
def filter(CONTAINER&& container, LAMBDA&& lambda) {
    CONTAINER&& w{};
    std::copy_if(container.begin(), container.end(), std::back_inserter(w), lambda);
    return w;
}

template <typename T>
def to_string(T t) {
  return std::to_string(t);
};

template <typename T, typename U>
def is_a(U u) {
  return std::holds_alternative<T>(u);
}

template <typename T, typename U>
def get_as(U u) {
  return std::get<T>(u);
}
#pragma endregion

#endif
