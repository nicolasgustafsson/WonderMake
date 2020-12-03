#pragma once
#include "Utilities/Container/ContainerTraits.h"
#include "Utilities/TypeTraits/ReturnType.h"
#include "Utilities/TypeTraits/ContainsType.h"
#include <type_traits>

template<typename TStorage>
class ContainerBackend
{
public:
	using StorageBackend = typename ContainerBackend<TStorage>;
	using IteratorType = typename TStorage::iterator;
	using ConstIteratorType = typename TStorage::const_iterator;

	void Clear()
	{
		myBackend.clear();
	}

	size_t Count() const
	{
		return myBackend.size();
	}
	
	TStorage myBackend;
};


template<typename TContainerBackend, typename TTrait>
class ImplementTrait {};

template<typename TContainerBackend>
class ImplementTrait<TContainerBackend, Iterable>
{
	using IteratorType = typename TContainerBackend::iterator;
	using ConstIteratorType = typename TContainerBackend::const_iterator;
	using ElementType = typename TContainerBackend::value_type;

	virtual typename IteratorType Erase(IteratorType aIt) = 0;
	virtual typename IteratorType Erase(ConstIteratorType aIt) = 0;

	virtual size_t Erase(const ElementType& aElementType) = 0;

	virtual typename ConstIteratorType cbegin() const = 0;
	virtual typename ConstIteratorType cend() const = 0;

	virtual typename IteratorType begin() = 0;
	virtual typename IteratorType end() = 0;
};

template<typename TContainerBackend>
class ImplementTrait<TContainerBackend, Indexable>
{
	using IteratorType = typename TContainerBackend::iterator;
	using ConstIteratorType = typename TContainerBackend::const_iterator;
	using ElementType = typename TContainerBackend::value_type;
	virtual ElementType& operator[](const size_t aIndex) = 0;
	virtual const ElementType& operator[](const size_t aIndex) const = 0;

	virtual IteratorType EraseAt(const size_t aIndex) = 0;
};

template<typename TContainerBackend, typename ... TTraits>
class ImplementTraits : ImplementTrait<TContainerBackend, TTraits>...
{
public:
	template<typename ... TOtherTraits>
	static constexpr bool SatisfiesTraits()
	{
		return IsSubsetOf<std::tuple<TOtherTraits...>, std::tuple<TTraits...>>::value;
	}

	template<typename TOtherTraits>
	static constexpr bool SatisfiesTraitsTuple()
	{
		return IsSubsetOf<TOtherTraits, std::tuple<TTraits...>>::value;
	}
};


template <typename TTraits, typename...>
struct FirstSatisfyingBackend
{
	using type = void;
	static constexpr bool found = false;
};

//[Nicos]: Loops through backends to find the first that satisfies the selected traits
template <typename TTraits, typename FirstBackend, typename... TBackends>
struct FirstSatisfyingBackend<TTraits, FirstBackend, TBackends...>
{
	using type = typename std::conditional<FirstBackend::template SatisfiesTraitsTuple<TTraits>() || !FirstSatisfyingBackend<TTraits, TBackends...>::found, FirstBackend, typename FirstSatisfyingBackend<TTraits, TBackends...>::type>::type;
	static constexpr bool found = FirstBackend::template SatisfiesTraitsTuple<TTraits>() || FirstSatisfyingBackend<TTraits, TBackends...>::found;
};