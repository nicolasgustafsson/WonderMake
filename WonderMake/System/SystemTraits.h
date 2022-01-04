#pragma once

#include <type_traits>

namespace SystemTraits
{
	template<typename... TTraits>
	struct Set
	{
		template<typename TTrait>
		static constexpr bool HasTrait = std::disjunction_v<std::is_same<TTrait, TTraits>...>;
	};

	struct Singleton {};
	struct Threadsafe {};
	struct Graphical {};
}

namespace STrait = SystemTraits;
