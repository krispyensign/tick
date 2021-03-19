#pragma once
#include "../base_types.hpp"
#include "../macros.hpp"
#include "rapidjson/stringbuffer.h"
#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/rapidjson.h>

using rapidjson::Document, rapidjson::Value, rapidjson::Writer,
    rapidjson::StringBuffer;
inline auto make_json(String data) -> rapidjson::Document {
  auto doc = rapidjson::Document();
  doc.Parse(data.c_str());
  if (doc.HasParseError()) {
    throw error("failed to parse: " + data);
  }
  return doc;
}

inline auto serialize(Document &doc) -> str {
  // setup the serialization context
  auto sb = StringBuffer();
  auto w = Writer(sb);
  doc.Accept(w);
  return sb.GetString();
}

#define docset_n(x, y) x[#y] = y;
#define docsetopt_n(x, y)                                                      \
  if ((y) != null)                                                             \
    (x)[#y] = (y).value();
#define docsetoptspec(x, y, z)                                                 \
  if ((y) != null)                                                             \
  (x)[#z] = (y).value()

#define docset(...) macro_dispatcher(docset, __VA_ARGS__)(__VA_ARGS__)
#define docset2(a, b) docset_n(a, b)
#define docset3(a, b, c) docset_n(a, b) docset_n(a, c)
#define docset4(a, b, ...) docset_n(a, b) docset3(a, __VA_ARGS__)
#define docset5(a, b, ...) docset_n(a, b) docset4(a, __VA_ARGS__)
#define docset6(a, b, ...) docset_n(a, b) docset5(a, __VA_ARGS__)
#define docset7(a, b, ...) docset_n(a, b) docset6(a, __VA_ARGS__)
#define docset8(a, b, ...) docset_n(a, b) docset7(a, __VA_ARGS__)
#define docset9(a, b, ...) docset_n(a, b) docset8(a, __VA_ARGS__)
#define docset10(a, b, ...) docset_n(a, b) docset9(a, __VA_ARGS__)
#define docset11(a, b, ...) docset_n(a, b) docset10(a, __VA_ARGS__)
#define docset12(a, b, ...) docset_n(a, b) docset11(a, __VA_ARGS__)
#define docset13(a, b, ...) docset_n(a, b) docset12(a, __VA_ARGS__)
#define docset14(a, b, ...) docset_n(a, b) docset13(a, __VA_ARGS__)
#define docset15(a, b, ...) docset_n(a, b) docset14(a, __VA_ARGS__)
#define docset16(a, b, ...) docset_n(a, b) docset15(a, __VA_ARGS__)
#define docset17(a, b, ...) docset_n(a, b) docset16(a, __VA_ARGS__)
#define docset18(a, b, ...) docset_n(a, b) docset17(a, __VA_ARGS__)

#define docsetopt(...) macro_dispatcher(docsetopt, __VA_ARGS__)(__VA_ARGS__)
#define docsetopt2(a, b) docsetopt_n(a, b)
#define docsetopt3(a, b, c) docsetopt_n(a, b) docsetopt_n(a, c)
#define docsetopt4(a, b, ...) docsetopt_n(a, b) docsetopt3(a, __VA_ARGS__)
#define docsetopt5(a, b, ...) docsetopt_n(a, b) docsetopt4(a, __VA_ARGS__)
#define docsetopt6(a, b, ...) docsetopt_n(a, b) docsetopt5(a, __VA_ARGS__)
#define docsetopt7(a, b, ...) docsetopt_n(a, b) docsetopt6(a, __VA_ARGS__)
#define docsetopt8(a, b, ...) docsetopt_n(a, b) docsetopt7(a, __VA_ARGS__)
#define docsetopt9(a, b, ...) docsetopt_n(a, b) docsetopt8(a, __VA_ARGS__)
#define docsetopt10(a, b, ...) docsetopt_n(a, b) docsetopt9(a, __VA_ARGS__)
#define docsetopt11(a, b, ...) docsetopt_n(a, b) docsetopt10(a, __VA_ARGS__)
#define docsetopt12(a, b, ...) docsetopt_n(a, b) docsetopt11(a, __VA_ARGS__)
#define docsetopt13(a, b, ...) docsetopt_n(a, b) docsetopt12(a, __VA_ARGS__)
#define docsetopt14(a, b, ...) docsetopt_n(a, b) docsetopt13(a, __VA_ARGS__)
#define docsetopt15(a, b, ...) docsetopt_n(a, b) docsetopt14(a, __VA_ARGS__)
#define docsetopt16(a, b, ...) docsetopt_n(a, b) docsetopt15(a, __VA_ARGS__)
#define docsetopt17(a, b, ...) docsetopt_n(a, b) docsetopt16(a, __VA_ARGS__)
#define docsetopt18(a, b, ...) docsetopt_n(a, b) docsetopt17(a, __VA_ARGS__)

#include "../unmacros.hpp"
