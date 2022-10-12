#pragma once

#include <tuple>
#include <type_traits>

template <typename TType, typename TTuple>
struct TupleContainsType
	: std::false_type
{};

template<typename TType, typename TRequiredFront, typename... TRequiredOthers>
struct TupleContainsType<TType, std::tuple<TRequiredFront, TRequiredOthers...>>
	: std::bool_constant<std::is_same_v<TType, TRequiredFront> || TupleContainsType<TType, std::tuple<TRequiredOthers...>>::value>
{};

template <typename TTuple, typename TRequiredTypesTuple>
struct TupleContainsAllTypesInTuple
	: std::false_type
{};

template<typename TRequiredTypesTuple, typename... TTypes>
struct TupleContainsAllTypesInTuple<std::tuple<TTypes...>, TRequiredTypesTuple>
	: std::bool_constant<std::conjunction_v<TupleContainsType<TTypes, TRequiredTypesTuple>...>>
{};
