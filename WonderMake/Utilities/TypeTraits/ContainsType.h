#pragma once
#include <tuple>
#include "Utilities/TypeTraits/ParameterPack.h"

template <class T, class... U>
struct Contains : std::disjunction<std::is_base_of<U, T>...> {};

template <typename...>
struct IsSubsetOf : std::false_type {};

template <typename... Types1, typename ... Types2>
struct IsSubsetOf<ParameterPack<Types1...>, ParameterPack<Types2...>> : std::conjunction<Contains<Types1, Types2...>...> {};

