#pragma once

#include "info.hpp"

#include <algorithm>
#include <iterator>
#include <rapidjson/allocators.h>
#include <rapidjson/document.h>
#include <rapidjson/rapidjson.h>
#include <string_view>
#include <tuple>
#include <utility>
#include <variant>

template <typename T>
void write_json(const T &value, rapidjson::Value &out_value,
                rapidjson::MemoryPoolAllocator<> alloc);

template <typename T>
void write_json_default(const T &value, rapidjson::Value &out_value,
                        rapidjson::MemoryPoolAllocator<> alloc) {
  if constexpr (Primitive<T>) {
    out_value = value;
  } else if constexpr (IsStruct<T>) {
    if (!out_value.IsObject())
      out_value.SetObject();
    std::apply(
        [&](auto... props) {
          (
              [&] {
                rapidjson::Value mem;
                write_json(value.*props.ptr_to_member, mem, alloc);
                out_value.AddMember(rapidjson::StringRef(props.name), mem,
                                    alloc);
              }(),
              ...);
        },
        info<T>::fields);
  } else if constexpr (IsVariant<T>) {
    out_value.SetObject();
    std::visit(
        [&]<typename U>(const U &value) {
          out_value["type"] = rapidjson::StringRef(info<U>::name);
          write_json(value, out_value, alloc);
        },
        value);
  } else {
    static_assert(false, "not defined");
  }
}

template <typename T>
void write_json(const T &value, rapidjson::Value &out_value,
                rapidjson::MemoryPoolAllocator<> alloc) {
  write_json_default<T>(value, out_value, alloc);
}

template <typename T>
bool read_json(T &value, const rapidjson::Value &in_value);

template <typename T>
bool read_json_default(T &value, const rapidjson::Value &in_value) {
  if constexpr (Primitive<T>) {
    if (!in_value.IsNumber())
      return false;
    value = in_value.Get<typeof(value)>();
    return true;
  } else if constexpr (IsStruct<T>) {
    if (!in_value.IsObject())
      return false;
    bool all_ok = true;
    std::apply(
        [&](auto... props) {
          (([&] {
             auto field = props;
             auto field_name = props.name;
             if (!in_value.HasMember(field_name)) {
               all_ok = false;
               return;
             };
             const rapidjson::Value &val = in_value[field_name];
             using prop_type = typeof(field);
             typename prop_type::member_t x;
             if (!read_json(x, val)) {
               all_ok = false;
               return;
             }
             value.*field.ptr_to_member = x;
           }()),
           ...);
        },
        info<T>::fields);
    return all_ok;
  } else if constexpr (IsVariant<T>) {
    if (!in_value.IsObject())
      return false;
    bool has_type = in_value.HasMember("type");
    const char *type_name = in_value["type"].GetString();
    variant_from_name(type_name, value);
    return std::visit(
        [&](auto &var_value) { return read_json(var_value, in_value); }, value);
  }
}

template <typename T>
bool read_json(T &value, const rapidjson::Value &in_value) {
  return read_json_default<T>(value, in_value);
}