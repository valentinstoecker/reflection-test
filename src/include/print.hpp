#pragma once

#include "info.hpp"
#include <cstddef>
#include <iostream>

template <typename T> void _print(const T& value, size_t indent) {
  static_assert(false && "print not implemented");
}

template<Primitive T>
void _print(const T& value, size_t indent) {
  std::cout << value << std::endl;
}

template<typename T>
void _print_field(const char* name, const T& value, size_t indent) {
  for (size_t i = 0; i < 2*indent; i++) std::cout << ' ';
  std::cout << name << ": ";
  _print(value, indent);
}

template<IsStruct T>
void _print(const T& value, size_t indent) {
  std::cout << std::endl;
  std::apply([&](auto... props){
    (_print_field(props.name, value.*props.ptr_to_member, indent + 1), ...);
  }, info<T>::fields);
};

template<typename T>
void print(const T& value) {
  _print(value, 0);
}