#pragma once
#include <tuple>

template <class T, class... U>
struct Contains : std::disjunction<std::is_same<T, U>...> {};

template <typename...>
struct IsSubsetOf : std::false_type {};

template <typename... Types1, typename ... Types2>
struct IsSubsetOf<std::tuple<Types1...>, std::tuple<Types2...>> : std::conjunction<Contains<Types1, Types2...>...> {};

