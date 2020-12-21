#pragma once
#include <concepts>
#include <type_traits>

#include <vector>
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
#include "Utilities/Container/BackendImplementations/UnorderedSetBackend.h"

template <typename T>
using IsBaseOfBackendTrait = std::is_base_of<BackendTrait, T>;

template <typename T>
using IsTemplateInstanceOfKey = IsTemplateInstanceOf<T, Key>;

struct Empty {};

template<typename TKey, typename TValue>
class KeyValuePair
{

};

namespace std {
	template <Hashable TKey, typename TValue>
	struct hash<KeyValuePair<TKey, TValue>>
	{
		size_t operator()(const KeyValuePair<TKey, TValue>& x) const
		{
			return hash(TKey);
		}
	};
}

template<typename TObjectType, typename TContainerBackends, ContainerTrait ... TContainerTraits>
class ContainerBase
{
	//[Nicos]: These need to be at the top so we can use Storage as a type
private:
	using TraitsPack = typename ResolvedImplications<TContainerTraits...>::type;
	using BackendTraitsPack = typename TypeFilter<IsBaseOfBackendTrait, TraitsPack>::type;
	using KeysPack = typename TypeFilter<IsTemplateInstanceOfKey, TraitsPack>::type;

	static_assert(ParameterPackSize(KeysPack()) < 2, "Container does not support more than 1 key!");

	constexpr static bool HasKey = ParameterPackSize(KeysPack()) > 0;

	template <typename ... TBackends>
	constexpr static auto StorageType(ParameterPack<TBackends...>)
	{
		static_assert(FirstSatisfyingBackend<BackendTraitsPack, TBackends...>::found, "No matching backend!");
		return FirstSatisfyingBackend<BackendTraitsPack, TBackends...>::type();
	}

	template <typename ... TKey>
	constexpr static auto KeyValueLinkContainerType(ParameterPack<TKey...>)
	{
		return Empty();
	}

	template <typename TKey>
	constexpr static auto KeyValueLinkContainerType(ParameterPack<TKey>)
	{
		return Container<KeyValuePair<decltype(TKey::KeyType()), TObjectType&>, Associative>();
	}

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

	using Storage = decltype(StorageType(TContainerBackends()));

	using KeyValueLinkContainer = decltype(KeyValueLinkContainerType(KeysPack()));
	using Key = decltype(KeyType(KeysPack()));

	Storage myBackend;

#pragma warning( push )
#pragma warning( disable : 4648 ) //[Nicos]: no unique address not currently supported by msvc, so until then our empty class will take up some space here
	[[no_unique_address]] KeyValueLinkContainer myKeyType;
#pragma warning( pop )

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
	void Add(TObjectTypeFunc aObject)
	{
		myBackend.Add<TObjectTypeFunc>(std::forward<TObjectTypeFunc>(aObject));
	}

	auto Find(const TObjectType& aObject) requires Iterable && CanEqualityCompare
	{
		auto it = std::find(begin(), end(), aObject);
		return it;
	}

	template<typename TObjectTypeFunc> requires std::is_same_v<TObjectType, std::decay_t<TObjectTypeFunc>> || std::is_constructible_v<TObjectType, TObjectTypeFunc>
	void AddUnique(TObjectTypeFunc aObject) requires Iterable && CanEqualityCompare
	{
		if (auto found = Find(aObject) != end())
			return;

		Add(std::forward<TObjectTypeFunc>(aObject));
	}

	void Clear()
	{
		myBackend.Clear();
	}

	size_t Count() const
	{
		return myBackend.Count();
	}

	constexpr TObjectType& operator[](const size_t aIndex) requires Indexable && !Associative 
	{
		return myBackend[aIndex];
	}

	constexpr const TObjectType& operator[](const size_t aIndex) const requires Indexable && !Associative
	{
		return myBackend[aIndex];
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
	, UnorderedSetBackend<TObjectType>
>, 
TContainerTraits...>;
