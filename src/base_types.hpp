#ifndef base_types_hpp
#define base_types_hpp

#include <optional>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

typedef int i16;
typedef unsigned int u16;
typedef long i32;
typedef unsigned long u32;
typedef long long i64;
typedef unsigned long long u64;
typedef float f32;
typedef double f64;
typedef char *c_str;
typedef std::string str;
const auto null = std::nullopt;
using error = std::runtime_error;
using AtomicBool = const std::atomic_bool&;
using String = const str&;

template <typename T>
using vec = std::vector<T, std::allocator<T>>;

template <typename... Ts>
using var = std::variant<Ts...>;

#endif
