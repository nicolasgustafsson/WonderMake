#pragma once
#include "Utilities/Container/ContainerBackend.h"
#include "Typedefs.h"

template <typename TObjectType>
class VectorBackend
	: public ContainerBackend<std::vector<TObjectType>>
	, public ImplementTraits<VectorBackend<TObjectType>, Iterable, Indexable, RandomAccess, Resizable, ContiguousElements>
{
public:
	using RawBackend = std::vector<TObjectType>;
	using Backend = ContainerBackend<std::vector<TObjectType>>;
	using IteratorType = typename Backend::IteratorType;
	using ConstIteratorType = typename Backend::ConstIteratorType;

	VectorBackend() = default;
	
	template<typename TIterator>
	VectorBackend(TIterator aFirst, TIterator aLast) noexcept(std::is_nothrow_constructible_v<Backend, TIterator, TIterator>)
		: Backend(aFirst, aLast)
	{}

	TObjectType& operator[](const size_t aIndex)
	{
		return this->myBackend[aIndex];
	}
	const TObjectType& operator[](const size_t aIndex) const
	{
		return this->myBackend[aIndex];
	}

	void Resize(const u64 aElements)
	{
		this->myBackend.resize(static_cast<size_t>(aElements));
	}
	
	size_t Erase(const TObjectType& aElementType)
	{
		auto it = std::remove(this->myBackend.begin(), this->myBackend.end(), aElementType);
		auto elementsRemoved = std::distance(it, this->myBackend.end());
		this->myBackend.erase(it, this->myBackend.end());

		return elementsRemoved;
	}

	IteratorType EraseAt(const size_t aIndex)
	{
		return Erase(this->myBackend.begin() + aIndex);
	}

	template<typename TPredicate>
	size_t EraseIf(TPredicate aPredicate)
	{
		auto it = std::remove_if(this->myBackend.begin(), this->myBackend.end(), aPredicate);
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

	template<typename TObjectTypeFunc> requires std::is_same_v<TObjectType, std::decay_t<TObjectTypeFunc>> || std::is_constructible_v<TObjectType, TObjectTypeFunc>
	void Add(TObjectTypeFunc aObjectType)
	{
		this->myBackend.push_back(std::forward<TObjectTypeFunc>(aObjectType));
	}

	TObjectType* data() noexcept
	{
		return this->myBackend.data();
	}

	const TObjectType* data() const noexcept
	{
		return this->myBackend.data();
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

