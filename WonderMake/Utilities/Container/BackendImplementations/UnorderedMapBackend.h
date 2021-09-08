#pragma once
#include <unordered_map>
#include "Utilities/Container/ContainerBackend.h"
#include "Typedefs.h"

template <typename TKeyType, typename TObjectType>
class UnorderedMapBackend
	: public ContainerBackend<std::unordered_map<TKeyType, TObjectType>>
	, public ImplementTraits<UnorderedMapBackend<TKeyType, TObjectType>, Associative, KeyIndex, Iterable, RandomAccess, StableElements>
{
public:
	using RawBackend = std::unordered_map<TKeyType, TObjectType>;
	using Backend = ContainerBackend<std::unordered_map<TKeyType, TObjectType>>;
	using IteratorType = typename Backend::IteratorType;
	using ConstIteratorType = typename Backend::ConstIteratorType;
	using ElementType = typename Backend::ElementType;

	template<typename TObjectTypeFunc> requires std::is_same_v<TObjectType, std::decay_t<TObjectTypeFunc>> || std::is_constructible_v<TObjectType, TObjectTypeFunc>
	void Add(const TKeyType aKey, TObjectTypeFunc aObjectType)
	{
		this->myBackend.insert(std::make_pair(aKey, std::forward<TObjectTypeFunc>(aObjectType)));
	}

	template<typename TPredicate>
	size_t EraseIf(TPredicate aPredicate)
	{
		auto elementsRemoved = std::erase_if(this->myBackend, aPredicate);
		return elementsRemoved;
	}

	[[nodiscard]] bool KeyExists(const TKeyType& aKey) const
	{
		return this->myBackend.find(aKey) != cend();
	}

	[[nodiscard]] TObjectType& Get(const TKeyType& aKey)
	{
		return (*(this->myBackend.find(aKey))).second;
	}

	[[nodiscard]] const TObjectType& Get(const TKeyType& aKey) const
	{
		return (*(this->myBackend.find(aKey))).second;
	}

	IteratorType Erase(const TKeyType& aKey)
	{
		return Erase(this->myBackend.find(aKey));
	}

	IteratorType Erase(IteratorType aIt)
	{
		return this->myBackend.erase(aIt);
	}

	IteratorType Erase(ConstIteratorType aIt)
	{
		return this->myBackend.erase(aIt);
	}

	TObjectType& operator[](const TKeyType& aKey)
	{
		return this->myBackend[aKey];
	}

	const TObjectType& operator[](const TKeyType& aKey) const
	{
		return this->myBackend[aKey];
	}

	IteratorType begin()
	{
		return this->myBackend.begin();
	}

	IteratorType end()
	{
		return this->myBackend.end();
	}

	ConstIteratorType cbegin() const
	{
		return this->myBackend.cbegin();
	}

	ConstIteratorType cend() const
	{
		return this->myBackend.cend();
	}
};

