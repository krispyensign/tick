#include "rapidjson/stringbuffer.h"
#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/rapidjson.h>

#include "json.hpp"

#include "../common/macros.hpp"

using rapidjson::Document, rapidjson::Value, rapidjson::Writer,
    rapidjson::StringBuffer, std::tuple, rapidjson::SizeType, std::get;
using str_lit = const char*;

template <typename W>
def add_member(W& writer, str_lit tag, String data) -> void {
  writer.String(tag);
  writer.String(data.c_str(), static_cast<SizeType>(data.length()));
}

template <typename W>
def add_member(W& writer, str_lit tag, i32 data) -> void {
  writer.String(tag);
  writer.Int(data);
}

template <typename W>
def add_member(W& writer, str_lit tag, f64 data) -> void {
  writer.String(tag);
  writer.Double(data);
}

template <typename W, typename T>
def add_member(W& writer, str_lit tag, Optional<T> data) -> void {
  if (data != null) {
    add_member(writer, tag, data.value());
  }
}

template<typename... T>
def serialize(T &&... args) -> str {
  mutant sb = StringBuffer();
  mutant w = Writer(sb);
  w.StartObject();
  ([&](let &input) {
    add_member(w, get<0>(input), get<1>(input)); }(args),
   ...);
  w.EndObject();
  return sb.GetString();
}
