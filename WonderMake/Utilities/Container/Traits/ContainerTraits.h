#pragma once
#include <type_traits>
#include "Utilities/Container/Traits/TraitList.h"

template<typename T>
concept ContainerTrait = std::is_base_of_v<BaseTrait, T>;

template<typename T, typename... Ts>
concept ContainsTrait = std::disjunction_v<std::is_same<T, Ts>...>;

template<typename TContainerBackend, typename TTrait>
class ImplementTrait {};

template<typename TContainerBackend>
class ImplementTrait<TContainerBackend, Iterable>
{

};

template<typename TContainerBackend>
class ImplementTrait<TContainerBackend, Indexable>
{

};

template<typename TContainerBackend, typename ... TTraits>
class ImplementTraits : public ImplementTrait<TContainerBackend, TTraits>...
{
public:
	template<typename ... TOtherTraits>
	static constexpr bool SatisfiesTraits()
	{
		return IsSubsetOf<ParameterPack<TOtherTraits...>, ResolvedImplications<TTraits...>::type>::value;
	}

	template<typename TOtherTraits>
	static constexpr bool SatisfiesTraitsPack()
	{
		return IsSubsetOf<TOtherTraits, ResolvedImplications<TTraits...>::type>::value;
	}
};
