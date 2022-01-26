#pragma once
#include <concepts>
#include <type_traits>

#include <vector>
#include <string>
#include <list>

#include "WonderMakeUtility/plf_colony.h"

#include "Traits/ContainerTraits.h"
#include "BackendImplementations/ColonyBackend.h"
#include "BackendImplementations/LinkedListBackend.h"
#include "BackendImplementations/VectorBackend.h"
#include "BackendImplementations/UnorderedMapBackend.h"

#include "Utilities/TypeTraits/ParameterPack.h"
#include "Utilities/Container/Traits/Implications.h"
#include "Utilities/TypeTraits/ContainsType.h"
#include "Utilities/TypeTraits/TypeTraits.h"
#include <json/json.hpp>

template <typename T>
using IsBaseOfBackendTrait = std::is_base_of<BackendTrait, T>;

template <typename T>
using IsTemplateInstanceOfKey = IsTemplateInstanceOf<T, Key>;

namespace _ContainerDetail
{
	struct Empty {};

	//helper type to allow the key type to be injected for unordered maps
	template<typename TObjectType, ContainerTrait ... TContainerTraits>
	struct ResolvedTypes
	{
		using TraitsPack = typename ResolvedImplications<TContainerTraits...>::type;
		using BackendTraitsPack = typename TypeFilter<IsBaseOfBackendTrait, TraitsPack>::type;
		using KeysPack = typename TypeFilter<IsTemplateInstanceOfKey, TraitsPack>::type;

		static_assert(ParameterPackSize(KeysPack()) < 2, "Container does not support more than 1 key!");

		template <typename ... TKey>
		inline constexpr static auto KeyType(ParameterPack<TKey...>)
		{
			return Empty();
		}

		template <typename TKey>
		inline constexpr static auto KeyType(ParameterPack<TKey>)
		{
			return TKey::KeyType();
		}

		using Key = decltype(KeyType(KeysPack()));
	};

	template<typename TObjectType, typename TContainerBackends, ContainerTrait ... TContainerTraits>
	struct ResolvedBackend
	{
		using ResolvedTypes = typename _ContainerDetail::ResolvedTypes<TObjectType, TContainerTraits...>;

		using TraitsPack = typename ResolvedTypes::TraitsPack;
		using BackendTraitsPack = typename ResolvedTypes::BackendTraitsPack;
		using KeysPack = typename ResolvedTypes::KeysPack;

		template <typename ... TBackends>
		inline constexpr static auto StorageType(ParameterPack<TBackends...>)
		{
			static_assert(FirstSatisfyingBackend<BackendTraitsPack, TBackends...>::found, "No matching backend!");
			return FirstSatisfyingBackend<BackendTraitsPack, TBackends...>::type();
		}

		using Storage = decltype(StorageType(TContainerBackends()));
	};
}

template<typename TObjectType, typename TContainerBackends, ContainerTrait ... TContainerTraits>
class ContainerBase : public _ContainerDetail::ResolvedBackend<TObjectType, TContainerBackends, TContainerTraits...>::Storage //[Nicos]: Derive from the selected storage
{
public:
	using Backend = _ContainerDetail::ResolvedBackend<TObjectType, TContainerBackends, TContainerTraits...>::Storage;
	using ResolvedTypes = typename _ContainerDetail::ResolvedTypes<TObjectType, TContainerTraits...>;
	using TraitsPack = typename ResolvedTypes::TraitsPack;

	using Backend::Backend;

	template<typename TBackend>
	inline constexpr bool HasBackend() const
	{
		return typeid(TBackend) == typeid(Backend);
	}

	template <typename T>
	constexpr static bool HasTrait = IsSubsetOf<ParameterPack<T>, TraitsPack>::value;
};

template<typename TObjectType, typename ... TContainerTraits>
using Container = ContainerBase<TObjectType, ParameterPack
	< ColonyBackend<TObjectType>
	, VectorBackend<TObjectType>
	, LinkedListBackend<TObjectType>
	, UnorderedMapBackend<typename _ContainerDetail::ResolvedTypes<TObjectType, TContainerTraits...>::Key, TObjectType>
>, 
TContainerTraits...>;


template<typename TObjectType, typename TContainerBackends, ContainerTrait ... TContainerTraits>
inline void to_json(json& aJson, const ContainerBase<TObjectType, TContainerBackends, TContainerTraits...>& aContainer) 
{
	aJson["Container"] = aContainer.myBackend;
}

template<typename TObjectType, typename TContainerBackends, ContainerTrait ... TContainerTraits>
inline void from_json(const json& aJson, ContainerBase<TObjectType, TContainerBackends, TContainerTraits...>& aContainer) 
{
	aContainer.myBackend = aJson["Container"].get<decltype(aContainer.myBackend)>();
}
