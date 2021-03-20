#pragma once
#include "../base_types.hpp"
#include "rapidjson/stringbuffer.h"
#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/rapidjson.h>

#include "../macros.hpp"

using rapidjson::Document, rapidjson::Value, rapidjson::Writer,
    rapidjson::StringBuffer, std::tuple, rapidjson::SizeType;
using str_lit = const char*;

template <typename W>
inline def add_member(W& writer, str_lit tag, String data) -> void{
  writer.String(tag);
  writer.String(data.c_str(), static_cast<SizeType>(data.length()));
}

template <typename W>
inline def add_member(W& writer, str_lit tag, i32 data) -> void {
  writer.String(tag);
  writer.Int(data);
}

template <typename W>
inline def add_member(W& writer, str_lit tag, f64 data) -> void {
  writer.String(tag);
  writer.Double(data);
}

template <typename W, typename T>
inline def add_member(W& writer, str_lit tag, const optional<T>& data) -> void {
  if (data != null) {
    add_member(writer, tag, data.value());
  }
}

template<typename... T>
inline def serialize(T &&... args) -> str {
  mutant sb = StringBuffer();
  mutant w = Writer(sb);
  w.StartObject();
  ([&](let &input) {
    add_member(w, get<0>(input), get<1>(input)); }(args),
   ...);
  w.EndObject();
  return sb.GetString();
}

inline def make_json(String data) -> Document {
  mutant doc = Document();
    doc.Parse(data.c_str());
    if (doc.HasParseError()) {
      throw error("failed to parse: " + data);
    }
  return doc;
}

#include "../unmacros.hpp"
