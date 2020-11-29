#pragma once
#include <concepts>
#include <type_traits>

#include <vector>
#include <list>

#include "Utilities/plf_colony.h"

#include "ContainerTraits.h"

template<typename TObjectType, ContainerTrait ... TContainerRequirements>
class Container
{
public:
	template <typename T>
	constexpr static bool HasTrait = ContainsTrait<T, TContainerRequirements...>;
	constexpr static bool Iterable = HasTrait<Iterable>;
	constexpr static bool Indexable = HasTrait<Indexable> || HasTrait<RandomAccess>;
	constexpr static bool RandomAccess = HasTrait<RandomAccess>;
	constexpr static bool StableElements = HasTrait<StableElements>;
	constexpr static bool ConstantInsertion = HasTrait<ConstantInsertion>;
	constexpr static bool ConstantDeletion = HasTrait<ConstantDeletion>;
	constexpr static bool CanEqualityCompare = std::equality_comparable<TObjectType>;

public:
	template<typename TObjectTypeFunc> requires std::is_same_v<TObjectType, std::decay_t<TObjectTypeFunc>> || std::is_constructible_v<TObjectType, TObjectTypeFunc>
	void Add(TObjectTypeFunc aObject)
	{
		if constexpr (UsesStorage<plf::colony<TObjectType>>)
		{
			myBackend.insert(std::forward<TObjectTypeFunc>(aObject));
		}
		else
		{
			myBackend.push_back(std::forward<TObjectTypeFunc>(aObject));
		}
	}

	//auto Erase(decltype(myBackend.begin()) aIterator) requires Iterable
	//{
	//	return myBackend.erase(aIterator);
	//}

	//template<class TPredicate>
	//void EraseIf(const TPredicate& aPred) requires Iterable
	//{
	//	  std::remove_if(begin(), end(), aPred);
	//}

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
		myBackend.clear();
	}

	constexpr TObjectType& operator[](const size_t aIndex) requires Indexable
	{
		if constexpr (UsesStorage<plf::colony<TObjectType>>)
		{
			return *myBackend.get_iterator_from_index(aIndex);
		}
		else
		{
			return myBackend[aIndex];
		}
	}

	constexpr const TObjectType& operator[](const size_t aIndex) const requires Indexable
	{
		if constexpr (UsesStorage<plf::colony<TObjectType>>)
		{
			return *myBackend.get_iterator_from_index(aIndex);
		}
		else
		{
			return myBackend[aIndex];
		}
	}

	void Sort() requires Iterable && RandomAccess
	{
		std::sort(begin(), end());
	}
	
	auto begin() const requires Iterable
	{
		return myBackend.begin();
	}

	auto end() const requires Iterable
	{
		return myBackend.end();
	}

	auto begin() requires Iterable
	{
		return myBackend.begin();
	}

	auto end() requires Iterable
	{
		return myBackend.end();
	}

	auto cbegin() const requires Iterable
	{
		return myBackend.cbegin();
	}

	auto cend() const requires Iterable
	{
		return myBackend.cend();
	}

	auto cbegin() requires Iterable
	{
		return myBackend.cbegin();
	}

	auto cend() requires Iterable
	{
		return myBackend.cend();
	}


private:
	//TODO: Change how this is handled? Make Storage class? either way this is not optimal
	static auto StorageType()
	{
		if constexpr (!RandomAccess && !ConstantInsertion)
			return plf::colony<TObjectType>();
		else if constexpr (!StableElements && !ConstantInsertion && !ConstantDeletion)
			return std::vector<TObjectType>();
		else if constexpr (!RandomAccess && !Indexable)
			return std::list<TObjectType>();
		else
			static_assert(false, "No container backend supports these traits!");
	}

	using Storage = decltype(StorageType());

	template <typename T>
	constexpr static bool UsesStorage = std::is_same_v<Storage, T>;

	Storage myBackend;
};