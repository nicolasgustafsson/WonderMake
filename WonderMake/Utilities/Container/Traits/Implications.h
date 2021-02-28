#pragma once

template<typename TExistingTraits, typename TFirst, typename TSecond>
constexpr auto TwoWayImplication() noexcept
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
constexpr auto ResolveImplications() noexcept
{
	auto base = TExistingTraits();

	/*Add implications from here*/
	auto sortImplication = TwoWayImplication<decltype(base), ParameterPack<Sortable>, ParameterPack<Iterable, RandomAccess>>();
	/*To here*/

	auto finalTraitCollection = sortImplication;

	constexpr bool needToGoDeeper = !std::is_same_v<decltype(base), decltype(finalTraitCollection)>;
	if constexpr (needToGoDeeper)
		return ResolveImplications<decltype(finalTraitCollection)>();

	return finalTraitCollection;
};

template<typename ... TTraits>
struct ResolvedImplications
{
	using type = decltype(ResolveImplications<ParameterPack<TTraits...>>());
};
