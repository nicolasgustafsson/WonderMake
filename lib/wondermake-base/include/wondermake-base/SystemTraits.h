#pragma once

#include <typeinfo>
#include <typeindex>
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

	namespace _Impl
	{
		template<typename TTuple>
		struct TupleTraits
		{};
		template<typename... TTraits>
		struct TupleTraits<std::tuple<TTraits...>>
		{
			using type = Set<TTraits...>;
		};
		inline constexpr auto ConcatTuples = [](auto&&... aTuples)
		{
			return std::tuple_cat(aTuples...);
		};
		template<typename TTraitSet>
		struct TraitSetTuple
		{};
		template<typename... TTraits>
		struct TraitSetTuple<Set<TTraits...>>
		{
			using type = std::tuple<TTraits...>;
		};

		template<typename TTraitSet>
		using TraitSetTuple_t = TraitSetTuple<TTraitSet>::type;

		template<typename... TTraitSets>
		struct ConcatTraitSets
		{
			using type = TupleTraits<std::decay_t<std::invoke_result_t<decltype(ConcatTuples), TraitSetTuple_t<TTraitSets>...>>>::type;
		};
	}

	template<typename... TTraitSets>
	using Concat = typename _Impl::ConcatTraitSets<TTraitSets...>::type;

	struct Foundational : public Trait {};
	struct Gui : public Trait {};
	struct Platform : public Trait {};
	struct Singleton : public Trait {};
	struct Threadsafe : public Trait {};
	struct WonderMake : public Trait {};
}

namespace STrait = SystemTraits;

using STFoundational		= SystemTraits::Foundational;
using STGui					= SystemTraits::Gui;
using STPlatform			= SystemTraits::Platform;
using STSingleton			= SystemTraits::Singleton;
using STThreadsafe			= SystemTraits::Threadsafe;
using STWonderMake			= SystemTraits::WonderMake;
