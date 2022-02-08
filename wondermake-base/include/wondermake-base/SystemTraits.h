#pragma once

#include <typeinfo>
#include <type_traits>
#include <unordered_set>

namespace SystemTraits
{
	class Trait {};

	template<typename TTrait>
	inline std::type_index ToObject()
	{
		static_assert(std::is_base_of_v<Trait, TTrait>, "Tried to convert non-trait type to trait object.");

		return typeid(TTrait);
	}

	using SetList = std::unordered_set<std::type_index>;

	template<typename... TTraits>
	struct Set
	{
		template<typename TTrait>
		static constexpr bool HasTrait = std::disjunction_v<std::is_same<TTrait, TTraits>...>;

		inline static SetList ToObject()
		{
			return { typeid(TTraits)... };
		}
	};

	struct Singleton : public Trait {};
	struct Threadsafe : public Trait {};
	struct PlatformInterface : public Trait {};
	struct Gui : public Trait {};
}

namespace STrait = SystemTraits;

using STSingleton			= SystemTraits::Singleton;
using STThreadsafe			= SystemTraits::Threadsafe;
using STPlatformInterface	= SystemTraits::PlatformInterface;
using STGui					= SystemTraits::Gui;
