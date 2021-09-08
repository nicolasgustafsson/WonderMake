#pragma once
#include "Utilities/plf_colony.h"
#include "Utilities/Container/ContainerBackend.h"

template <typename TObjectType>
class ColonyBackend
	: public ContainerBackend<plf::colony<TObjectType>>
	, public ImplementTraits<ColonyBackend<TObjectType>, Iterable, Indexable, StableElements>
{
public:
	using RawBackend = plf::colony<TObjectType>;
	using Backend = ContainerBackend<plf::colony<TObjectType>>;
	using IteratorType = typename Backend::IteratorType;
	using ConstIteratorType = typename Backend::ConstIteratorType;
	using ElementType = typename Backend::ElementType;

	TObjectType& operator[](const size_t aIndex)
	{
		return *(this->myBackend.get_iterator_from_index(aIndex));
	}
	const TObjectType& operator[](const size_t aIndex) const
	{
		return *(this->myBackend.get_iterator_from_index(aIndex));
	}

	IteratorType EraseAt(const size_t aIndex)
	{
		return Erase(this->myBackend.get_iterator_from_index(aIndex));
	}

	template<typename TPredicate>
	size_t EraseIf(TPredicate aPredicate)
	{
		auto it = std::remove_if(this->myBackend.begin(), this->myBackend.end(), aPredicate);
		auto elementsRemoved = std::distance(it, this->myBackend.end());
		this->myBackend.erase(it, this->myBackend.end());
		return elementsRemoved;
	}

	size_t Erase(const TObjectType& aElementType)
	{
		auto it = std::remove(this->myBackend.begin(), this->myBackend.end(), aElementType);
		auto elementsRemoved = std::distance(it, this->myBackend.end());
		this->myBackend.erase(it, this->myBackend.end());

		return elementsRemoved;
	}

	template<typename TObjectTypeFunc> requires std::is_same_v<TObjectType, std::decay_t<TObjectTypeFunc>> || std::is_constructible_v<TObjectType, TObjectTypeFunc>
	auto&& Add(TObjectTypeFunc aObjectType)
	{
		auto&& iterator = this->myBackend.insert(std::forward<TObjectTypeFunc>(aObjectType));
		return *iterator;
	}

	template<typename ... TObjectTypes>
	auto&& Emplace(TObjectTypes... aObjectType)
	{
		auto&& iterator = this->myBackend.emplace(std::forward<TObjectTypes>(aObjectType)...);
		 
		return *iterator;
	}

	IteratorType Erase(IteratorType aIt)
	{
		return this->myBackend.erase(aIt);
	}

	IteratorType Erase(ConstIteratorType aIt)
	{
		return this->myBackend.erase(aIt);
	}

	IteratorType begin()
	{
		return this->myBackend.begin();
	}

	IteratorType end()
	{
		return this->myBackend.end();
	}

	ConstIteratorType begin() const
	{
		return this->myBackend.begin();
	}

	ConstIteratorType end() const
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

