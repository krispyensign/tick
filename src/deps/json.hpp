#pragma once
#include "../common/base_types.hpp"
#include <rapidjson/document.h>
#include "../common/macros.hpp"

inline def make_json(String data) -> rapidjson::Document {
  mutant doc = rapidjson::Document();
  doc.Parse(data.c_str());
  if (doc.HasParseError()) {
    throw error("failed to parse: " + data);
  }
  return doc;
}
template<typename... T>

def serialize(T &&... args) -> str;

#include "../common/unmacros.hpp"
