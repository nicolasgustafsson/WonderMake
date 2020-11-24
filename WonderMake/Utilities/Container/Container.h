#pragma once
#include <vector>
#include "Utilities/plf_colony.h"
#include <concepts>
#include <type_traits>
#include <list>
#include "ContainerTraits.h"

//TODO: Several data types? can go with tuple for now tho but may want SoA
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

	auto Add(TObjectType aObject)
	{
		if constexpr (UsesStorage<plf::colony<TObjectType>>)
		{
			return myBackend.insert(aObject);
		}
		else
		{
			return myBackend.push_back(aObject);
		}
	}

	//auto Erase(decltype(myBackend.begin()) aIterator) requires Iterable
	//{
	//	return myBackend.erase(aIterator);
	//}

	template<class TPredicate>
	void EraseIf(TPredicate&& aPred) requires Iterable
	{
		//std::remove_if(begin(), end(), aPred);
	}

	TObjectType* Find(const TObjectType& aObject) requires Iterable && CanEqualityCompare
	{
		auto it = std::find(begin(), end(), aObject);
		if (it == end())
			return nullptr;

		return &*it;
	}

	auto AddUnique(TObjectType aObject) requires Iterable && CanEqualityCompare
	{
		if (TObjectType* found = Find(aObject))
			return *found;

		if constexpr (UsesStorage<plf::colony<TObjectType>>)
		{
			return *myBackend.insert(aObject);
		}
		else
		{
			return *myBackend.push_back(aObject);
		}
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

	Storage myBackend;
};