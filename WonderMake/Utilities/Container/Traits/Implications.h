#pragma once


template<typename TExistingTraits, typename TFirst, typename TSecond>
constexpr auto Implication()
{
	if constexpr (IsSubsetOf<TFirst, TExistingTraits>::value && IsSubsetOf<TSecond, TExistingTraits>::value)
		return TExistingTraits();
	else if constexpr (IsSubsetOf<TFirst, TExistingTraits>::value)
		return decltype(UniqueParameterPackFun(Concatenate_t<TExistingTraits, TSecond>()))();
	else if constexpr (IsSubsetOf<TSecond, TExistingTraits>::value)
		return decltype(UniqueParameterPackFun(Concatenate_t<TExistingTraits, TFirst>()))();
	else
		return TExistingTraits();
};

template<typename TExistingTraits>
constexpr auto ResolveImplications()
{
	auto SortImplication = Implication<TExistingTraits, ParameterPack<Sortable>, ParameterPack<Iterable, RandomAccess>>();

	return SortImplication;
};

template<typename ... TTraits>
struct ResolvedImplications
{
	//using type = typename Concatenate<ParameterPack<TTraits...>, ParameterPack<Iterable>>::type;

	using type = decltype(ResolveImplications<ParameterPack<TTraits...>>());
};
