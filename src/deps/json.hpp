#pragma once
#include <rapidjson/document.h>
#include <rapidjson/rapidjson.h>
#include <rapidjson/prettywriter.h>
#include "../base_types.hpp"
#include "rapidjson/stringbuffer.h"
using rapidjson::Document, rapidjson::Value, rapidjson::PrettyWriter, rapidjson::StringBuffer;
inline auto make_json(String data) -> rapidjson::Document {
  auto doc = rapidjson::Document();
  doc.Parse(data.c_str());
  if (doc.HasParseError()) {
    throw error("failed to parse: " + data);
  }
  return doc;
}

#define S_STRING(x, y) (x).String(#y); (x).String((y).c_str(), (y).length());
#define S_OPTSTR(x, y) \
  if((y) != null) { \
    (x).String(#y); \
    (x).String((y).value().c_str(), (y).value().length()); \
  }
#define S_OPTSTR2(x, y, z) \
  if((y) != null) { \
    (x).String(z); \
    (x).String((y).value().c_str(), (y).value().length()); \
  }

#define S_OPTINT(x, y) \
  if((y) != null) { \
    (x).String(#y); \
    (x).Int((y).value()); \
  }
#define S_DOUBLE(x, y) (x).String(#y); (x).Double(y);
#define S_OPTDOU(x, y) \
  if((y) != null) { \
    (x).String(#y); \
    (x).Double((y).value()); \
  };
#define S_OPTDOU2(x, y, z) \
  if((y) != null) { \
    (x).String(z); \
    (x).Double((y).value()); \
  };
