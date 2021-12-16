#pragma once
#include "Utilities/Container/Traits/ContainerTraits.h"
#include "Utilities/TypeTraits/ContainsType.h"
#include <type_traits>

template<typename TStorage>
class ContainerBackend
{
public:
#ifdef __GNUC__
	using StorageBackend = ContainerBackend<TStorage>;
#else
    using StorageBackend = typename ContainerBackend<TStorage>;
#endif
	using IteratorType = typename TStorage::iterator;
	using ConstIteratorType = typename TStorage::const_iterator;
	using ElementType = typename TStorage::value_type;

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
	using type = typename std::conditional<FirstBackend::template SatisfiesTraitsPack<TTraits>() || !FirstSatisfyingBackend<TTraits, TBackends...>::found, FirstBackend, typename FirstSatisfyingBackend<TTraits, TBackends...>::type>::type;
	static constexpr bool found = FirstBackend::template SatisfiesTraitsPack<TTraits>() || FirstSatisfyingBackend<TTraits, TBackends...>::found;
};