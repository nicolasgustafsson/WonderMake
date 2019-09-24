#pragma once
#include "Typedefs.h"

#include "Components/ComponentSystem.h"

#include "Functionalities/FunctionalitySystem.h"

#include "System/SystemPtr.h"

#include <typeindex>

struct SComponent;
class _BaseFunctionality;

class Object
{
public:
	Object() = default;
	virtual ~Object();

	// Will not add the type if it already exists.
	template<typename TType>
	inline TType& Add();

	// Will not remove the type if existing functionalities depend on it.
	template<typename TType>
	inline void Remove();

	template<typename TVisitFunc>
	inline void Visit(TVisitFunc aVisitFunc);

private:
	template<typename TType>
	struct SRefCounter
	{
		constexpr SRefCounter(TType* aReference) noexcept
			: Reference(aReference)
			, RefCount(1)
		{}

		TType* Reference = nullptr;
		i32 RefCount = 0;
	};

	template<typename TType>
	using Pair = std::pair<std::type_index, SRefCounter<TType>>;
	template<typename TType>
	using PairList = std::vector<Pair<TType>>;

	using FunctionalityList = PairList<_BaseFunctionality>;
	using ComponentList = PairList<SComponent>;

	template<typename TType, typename TBaseType, typename TCreateFunc>
	inline TType& Add(PairList<TBaseType>& aList, TCreateFunc aCreateFunc);

	template<typename TType, typename TBaseType>
	inline TType* Remove(PairList<TBaseType>& aList);

	FunctionalityList myFunctionalities;
	ComponentList myComponents;
};

template<typename TType>
inline TType& Object::Add()
{
	if constexpr (std::is_base_of<SComponent, TType>::value)
	{
		return Add<TType>(myComponents, []() -> TType &
			{
				return SystemPtr<ComponentSystem<TType>>()->AddComponent();
			});
	}
	else if constexpr (std::is_base_of<_BaseFunctionality, TType>::value)
	{
		return Add<TType>(myFunctionalities, [&]() -> TType &
			{
				return SystemPtr<FunctionalitySystem<TType>>()->AddFunctionality(*this);
			});
	}
	else
	{
		static_assert("Type must inherit from SComponent or _BaseFunctionality!");
	}
}

template<typename TType>
inline void Object::Remove()
{
	if constexpr (std::is_base_of<SComponent, TType>::value)
	{
		Remove<TType>(myComponents);
	}
	else if constexpr (std::is_base_of<_BaseFunctionality, TType>::value)
	{
		auto functionality = Remove<TType>(myFunctionalities);

		functionality->Destroy(*this);
	}
	else
	{
		static_assert("Type must inherit from SComponent or _BaseFunctionality!");
	}
}

template<typename TVisitFunc>
inline void Object::Visit(TVisitFunc aVisitFunc)
{
	for (auto& functionality : myFunctionalities)
	{
		aVisitFunc(functionality.first, functionality.second.Reference);
	}
	for (auto& component : myComponents)
	{
		aVisitFunc(component.first, component.second.Reference);
	}
}

template<typename TType, typename TBaseType, typename TCreateFunc>
inline TType& Object::Add(PairList<TBaseType>& aList, TCreateFunc aCreateFunc)
{
	const auto it = std::find_if(aList.begin(), aList.end(), [](const auto& aPair)
		{
			return aPair.first == typeid(TType);
		});

	if (it != aList.cend())
	{
		auto& counter = it->second;

		counter.RefCount++;

		return static_cast<TType&>(*counter.Reference);
	}

	auto& added = aCreateFunc();

	aList.emplace_back(Pair<TBaseType>(typeid(TType), &added));

	return added;
}

template<typename TType, typename TBaseType>
inline TType* Object::Remove(PairList<TBaseType>& aList)
{
	const auto it = std::find_if(aList.begin(), aList.end(), [](const auto& aPair)
		{
			return aPair.first == typeid(TType);
		});

	if (it == aList.cend())
	{
		return nullptr;
	}

	auto& counter = it->second;
	auto reference = static_cast<TType*>(counter.Reference);

	assert(counter.RefCount != 0);

	counter.RefCount--;

	if (counter.RefCount == 0)
	{
		aList.erase(it);
	}

	return reference;
}
