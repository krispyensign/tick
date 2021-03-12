#pragma once

#include <exception>
#include <optional>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>
#include <thread>
#include <chrono>
#include <atomic>
#include <functional>

using i16 = int;
using u16 = unsigned int;
using i32 = long;
using u32 = unsigned long;
using i64 = long long;
using u64 = unsigned long long;
using f32 = float;
using f64 = double;
using c_str = char *;
using str = std::string;
const auto null = std::nullopt;
using error = std::runtime_error;
using AtomicBool = const std::atomic_bool&;
using String = const str&;
using Exception = const std::exception&;
using std::optional, std::function;
using namespace std::chrono_literals;
namespace this_thread = std::this_thread;

template <typename T>
using vec = std::vector<T, std::allocator<T>>;

template <typename... Ts>
using var = std::variant<Ts...>;
