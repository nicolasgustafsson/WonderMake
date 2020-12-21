#pragma once
#include "Utilities/Container/ContainerBackend.h"

template <typename TObjectType>
class LinkedListBackend
	: public ContainerBackend<std::list<TObjectType>>
	, public ImplementTraits<LinkedListBackend<TObjectType>, Iterable, StableElements, ConstantInsertion, ConstantDeletion>
{
public:
	using RawBackend = std::list<TObjectType>;
	using Backend = ContainerBackend<std::list<TObjectType>>;
	using IteratorType = typename Backend::IteratorType;
	using ConstIteratorType = typename Backend::ConstIteratorType;

	template<typename TObjectTypeFunc> requires std::is_same_v<TObjectType, std::decay_t<TObjectTypeFunc>> || std::is_constructible_v<TObjectType, TObjectTypeFunc>
	void Add(TObjectTypeFunc aObjectType)
	{
		this->myBackend.push_back(std::forward<TObjectTypeFunc>(aObjectType));
	}

	size_t Erase(const TObjectType& aElementType)
	{
		auto it = std::remove(this->myBackend.begin(), this->myBackend.end(), aElementType);
		auto elementsRemoved = std::distance(it, this->myBackend.end());
		this->myBackend.erase(it, this->myBackend.end());

		return elementsRemoved;
	}

	IteratorType Erase(IteratorType aIt)
	{
		return this->myBackend.erase(aIt);
	}

	IteratorType Erase(ConstIteratorType aIt)
	{
		return this->myBackend.erase(aIt);
	}

	template<typename TPredicate>
	size_t EraseIf(TPredicate aPredicate)
	{
		auto it = std::remove_if(this->myBackend.begin(), this->myBackend.end(), aPredicate);
		auto elementsRemoved = std::distance(it, this->myBackend.end());
		this->myBackend.erase(it, this->myBackend.end());
		return elementsRemoved;
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

