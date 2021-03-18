#pragma once
#include <rapidjson/document.h>
#include <rapidjson/rapidjson.h>
#include <rapidjson/prettywriter.h>
#include "../base_types.hpp"
#include "rapidjson/stringbuffer.h"
using rapidjson::Document, rapidjson::Value, rapidjson::Writer, rapidjson::StringBuffer;
inline auto make_json(String data) -> rapidjson::Document {
  auto doc = rapidjson::Document();
  doc.Parse(data.c_str());
  if (doc.HasParseError()) {
    throw error("failed to parse: " + data);
  }
  return doc;
}
