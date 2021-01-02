#pragma once
#include <concepts>
#include <type_traits>

#include <vector>
#include <string>
#include <list>

#include "Utilities/plf_colony.h"

#include "Traits/ContainerTraits.h"
#include "BackendImplementations/ColonyBackend.h"
#include "BackendImplementations/LinkedListBackend.h"
#include "BackendImplementations/VectorBackend.h"

#include "Utilities/TypeTraits/ParameterPack.h"
#include "Utilities/Container/Traits/Implications.h"
#include "Utilities/TypeTraits/ContainsType.h"
#include "Utilities/TypeTraits/TypeTraits.h"
#include "BackendImplementations/UnorderedMapBackend.h"

template <typename T>
using IsBaseOfBackendTrait = std::is_base_of<BackendTrait, T>;

template <typename T>
using IsTemplateInstanceOfKey = IsTemplateInstanceOf<T, Key>;

struct Empty {};

namespace _ContainerDetail
{
	template<typename TObjectType, ContainerTrait ... TContainerTraits>
	struct ResolvedTypes
	{
		using TraitsPack = typename ResolvedImplications<TContainerTraits...>::type;
		using BackendTraitsPack = typename TypeFilter<IsBaseOfBackendTrait, TraitsPack>::type;
		using KeysPack = typename TypeFilter<IsTemplateInstanceOfKey, TraitsPack>::type;

		static_assert(ParameterPackSize(KeysPack()) < 2, "Container does not support more than 1 key!");

		template <typename ... TKey>
		constexpr static auto KeyType(ParameterPack<TKey...>)
		{
			return Empty();
		}

		template <typename TKey>
		constexpr static auto KeyType(ParameterPack<TKey>)
		{
			return TKey::KeyType();
		}

		using Key = decltype(KeyType(KeysPack()));
	};
}

template<typename TObjectType, typename TContainerBackends, ContainerTrait ... TContainerTraits>
class ContainerBase
{
	//[Nicos]: These need to be at the top so we can use Storage as a type
private:

	using ResolvedTypes = typename _ContainerDetail::ResolvedTypes<TObjectType, TContainerTraits...>;

	using TraitsPack = typename ResolvedTypes::TraitsPack;
	using BackendTraitsPack = typename ResolvedTypes::BackendTraitsPack;
	using KeysPack = typename ResolvedTypes::KeysPack;

	constexpr static bool HasKey = ParameterPackSize(KeysPack()) > 0;

	template <typename ... TBackends>
	constexpr static auto StorageType(ParameterPack<TBackends...>)
	{
		static_assert(FirstSatisfyingBackend<BackendTraitsPack, TBackends...>::found, "No matching backend!");
		return FirstSatisfyingBackend<BackendTraitsPack, TBackends...>::type();
	}

	using Storage = decltype(StorageType(TContainerBackends()));
	using Key = typename ResolvedTypes::Key;

	Storage myBackend;

public:

	template <typename T>
	constexpr static bool HasTrait = IsSubsetOf<ParameterPack<T>, TraitsPack>::value;
	constexpr static bool Iterable = HasTrait<Iterable>;
	constexpr static bool Indexable = HasTrait<Indexable> || HasTrait<RandomAccess>;
	constexpr static bool RandomAccess = HasTrait<RandomAccess>;
	constexpr static bool Associative = HasTrait<Associative>;
	constexpr static bool StableElements = HasTrait<StableElements>;
	constexpr static bool ConstantInsertion = HasTrait<ConstantInsertion>;
	constexpr static bool ConstantDeletion = HasTrait<ConstantDeletion>;
	constexpr static bool Sortable = HasTrait<Sortable>;
	constexpr static bool CanEqualityCompare = std::equality_comparable<TObjectType>;

	template<typename TBackend>
	constexpr bool HasBackend() const 
	{
		return typeid(TBackend) == typeid(Storage);
	}

	template<typename TObjectTypeFunc> requires std::is_same_v<TObjectType, std::decay_t<TObjectTypeFunc>> || std::is_constructible_v<TObjectType, TObjectTypeFunc>
	void Add(TObjectTypeFunc aObject) requires !HasKey
	{
		myBackend.Add<TObjectTypeFunc>(std::forward<TObjectTypeFunc>(aObject));
	}

	template<typename TObjectTypeFunc> requires std::is_same_v<TObjectType, std::decay_t<TObjectTypeFunc>> || std::is_constructible_v<TObjectType, TObjectTypeFunc>
	void Add(const Key& aKey, TObjectTypeFunc aObject) requires HasKey
	{
		myBackend.Add<TObjectTypeFunc>(aKey, std::forward<TObjectTypeFunc>(aObject));
	}

	[[nodiscard]] bool KeyExists(const Key& aKey) requires HasKey
	{
		return myBackend.KeyExists(aKey);
	}

	[[nodiscard]] TObjectType& Get(const Key& aKey) requires HasKey
	{
		return myBackend.Get(aKey);
	}

	auto Find(const TObjectType& aObject) requires Iterable && CanEqualityCompare
	{
		auto it = std::find(begin(), end(), aObject);
		return it;
	}

	template<typename TObjectTypeFunc> requires std::is_same_v<TObjectType, std::decay_t<TObjectTypeFunc>> || std::is_constructible_v<TObjectType, TObjectTypeFunc>
	void AddUnique(TObjectTypeFunc aObject) requires Iterable && CanEqualityCompare && !HasKey
	{
		if (auto found = Find(aObject) != end())
			return;

		Add(std::forward<TObjectTypeFunc>(aObject));
	}

	template<typename TObjectTypeFunc> requires std::is_same_v<TObjectType, std::decay_t<TObjectTypeFunc>> || std::is_constructible_v<TObjectType, TObjectTypeFunc>
	void AddUnique(const Key& aKey, TObjectTypeFunc aObject) requires Iterable && CanEqualityCompare && HasKey
	{
		if (auto found = Find(aObject) != end())
			return;

		Add(aKey, std::forward<TObjectTypeFunc>(aObject));
	}

	void Clear()
	{
		myBackend.Clear();
	}

	size_t Count() const
	{
		return myBackend.Count();
	}

	constexpr TObjectType& operator[](const size_t aIndex) requires Indexable && !HasKey
	{
		return myBackend[aIndex];
	}

	constexpr const TObjectType& operator[](const size_t aIndex) const requires Indexable && !HasKey
	{
		return myBackend[aIndex];
	}

	constexpr TObjectType& operator[](const Key& aKey) requires HasKey && !Indexable
	{
		return myBackend[aKey];
	}

	constexpr const TObjectType& operator[](const Key& aKey) const requires HasKey && !Indexable
	{
		return myBackend[aKey];
	}

	void Sort() requires Sortable
	{
		std::sort(begin(), end());
	}
	
	auto cbegin() const requires Iterable
	{
		return myBackend.cbegin();
	}

	auto cend() const requires Iterable
	{
		return myBackend.cend();
	}

	auto begin() requires Iterable
	{
		return myBackend.begin();
	}

	auto end() requires Iterable
	{
		return myBackend.end();
	}

	auto Erase(TObjectType aElement) requires Iterable
	{
		return myBackend.Erase(aElement);
	}

	auto Erase(typename Storage::IteratorType aIterator) requires Iterable
	{
		return myBackend.Erase(aIterator);
	}

	auto Erase(typename Storage::ConstIteratorType aIterator) requires Iterable
	{
		return myBackend.Erase(aIterator);
	}

	auto EraseAt(const size_t aIndex) requires Indexable
	{
		return myBackend.EraseAt(aIndex);
	}

	template<typename TPredicate>
	size_t EraseIf(TPredicate aPredicate) requires Iterable
	{
		return myBackend.EraseIf(aPredicate);
	}
};

template<typename TObjectType, typename ... TContainerTraits>
using Container = ContainerBase<TObjectType, ParameterPack<ColonyBackend<TObjectType>
	, VectorBackend<TObjectType>
	, LinkedListBackend<TObjectType>
	, UnorderedMapBackend<typename _ContainerDetail::ResolvedTypes<TObjectType, TContainerTraits...>::Key, TObjectType>
>, 
TContainerTraits...>;
