#pragma once

#include <algorithm>
#include <array>
#include <concepts>
#include <cstddef>
#include <format>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

template <size_t N> struct string_literal {
  constexpr string_literal(const char (&str)[N]) { std::copy_n(str, N, value); }
  char value[N];
};

template <string_literal NAME, typename T> struct named {
  constexpr static const char *name = NAME.value;
  using self_type = T;
};

template <typename T> struct info {};

template <typename S, typename M> struct prop {
  const char *name;
  M S::*ptr_to_member;
  using struct_t = S;
  using member_t = M;

  constexpr prop(const char *n, M S::*p) : name(n), ptr_to_member(p) {}
};

#define INFO(struct_name)                                                      \
  template <> struct info<struct_name> : named<#struct_name, struct_name>

#define _FIELD(field_name) prop(#field_name, &self_type::field_name)

#define EMPTY()
#define DEFER(id) id EMPTY()
#define OBSTRUCT(...) __VA_ARGS__ DEFER(EMPTY)()
#define EXPAND(...) __VA_ARGS__

#define EVAL(...) EVAL1(EVAL1(EVAL1(__VA_ARGS__)))
#define EVAL1(...) EVAL2(EVAL2(EVAL2(__VA_ARGS__)))
#define EVAL2(...) EVAL3(EVAL3(EVAL3(__VA_ARGS__)))
#define EVAL3(...) EVAL4(EVAL4(EVAL4(__VA_ARGS__)))
#define EVAL4(...) __VA_ARGS__

#define FIELDS(...)                                                            \
  constexpr static auto fields = std::make_tuple(_FE(__VA_ARGS__))
#define _FE(...) EVAL(_FE_1(__VA_ARGS__))
#define _FE_1(a, ...) _FIELD(a) __VA_OPT__(, OBSTRUCT(_FE_AGAIN)()(__VA_ARGS__))
#define _FE_AGAIN() _FE_1

struct validator_result {
  std::string condition;
};

using validator_results = std::vector<validator_result>;

#define VALIDATOR                                                              \
  static void validate(const self_type &self, validator_results &results)

#define CHECK(COND)                                                            \
  if (COND)                                                                    \
    results.emplace_back(std::string(#COND));

#define CHECK_LE(x, y)                                                         \
  if (x <= y)                                                                  \
    results.emplace_back(                                                      \
        std::format(#x " <= " #y " ({} <= {}) not satisfied", x, y));

template <typename T>
concept IsStruct = requires { info<std::remove_cvref_t<T>>::fields; };

template <typename T>
concept Primitive = std::is_arithmetic_v<std::remove_cvref_t<T>> ||
                    std::is_enum_v<std::remove_cvref_t<T>>;

template <typename T>
concept IsVariant =
    requires(const T &value) { std::visit([](const auto & val) {}, value); };

template <typename... Ts>
void variant_from_name(const char *name, std::variant<Ts...> &result) {
  using variant_type = std::variant<Ts...>;
  ([&]<size_t... indices>(std::index_sequence<indices...> _) {
    (([&] {
       const char *alt_name =
           info<std::variant_alternative_t<indices, variant_type>>::name;
       if (std::string_view(alt_name) == std::string_view(name)) {
         result = variant_type(std::in_place_index_t<indices>{});
       }
     }()),
     ...);
  })(std::index_sequence_for<Ts...>{});
}
