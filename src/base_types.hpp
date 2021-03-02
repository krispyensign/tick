#ifndef base_types_hpp
#define base_types_hpp

#include <stdexcept>
#include <string>
#include <variant>
#include <vector>
using namespace std;

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

using error = runtime_error;

template <typename T>
using vec = vector<T, allocator<T>>;

template <typename... Ts>
using var = variant<Ts...>;

#endif
