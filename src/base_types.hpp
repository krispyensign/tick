#ifndef base_types_hpp
#define base_types_hpp

#include <string>
#include <variant>
using namespace std;

#pragma region base_type_aliases
typedef int i16;
typedef unsigned int u16;
typedef long i32;
typedef unsigned long u32;
typedef long long i64;
typedef unsigned long long u64;
typedef float f32;
typedef double f64;
typedef char *c_str;
typedef string str;
#pragma endregion

#pragma region namespace_aliases

using error = runtime_error;

template <typename T>
using vec = vector<T, allocator<T>>;

template <typename... Ts>
using var = variant<Ts...>;

#endif
