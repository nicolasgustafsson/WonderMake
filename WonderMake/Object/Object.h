#pragma once
#include "WonderMakeBase/Typedefs.h"

#include "Object/DependencyDestructor.h"

#include <typeindex>

struct SComponent;
class _BaseFunctionality;
class SystemAbstracted;

class Object final
	: public NonMovable
	, public NonCopyable
{
public:
	Object(Object&& aOther);
	Object& operator=(Object&& aOther);
	Object() = default;
	~Object() noexcept;

	// Will not add the type if it already exists.
	template<typename TType, typename TCreateFunc>
	inline TType& Add(TCreateFunc aCreateFunc, DependencyDestructor& aDestructor, const bool aExplicitlyAdded = true);

	// Will not remove the type if existing functionalities depend on it.
	template<typename TType>
	inline void Remove(const bool aExplicitlyRemoved = true);

	template<typename TVisitFunc>
	inline void Visit(TVisitFunc aVisitFunc);

private:
	template<typename TType>
	struct SRefCounter
	{
		constexpr SRefCounter(TType* const aReference, DependencyDestructor* const aDestructor, const bool aExplicitlyAdded) noexcept
			: Reference(aReference)
			, Destructor(aDestructor)
			, RefCount(aExplicitlyAdded ? 0 : 1)
			, ExplicitlyAdded(aExplicitlyAdded)
		{}

		TType*					Reference = nullptr;
		DependencyDestructor*	Destructor = nullptr;
		i32						RefCount = 0;
		bool					ExplicitlyAdded = false;
	};

	template<typename TType>
	using Pair = std::pair<std::type_index, SRefCounter<TType>>;
	template<typename TType>
	using PairList = std::vector<Pair<TType>>;

	using FunctionalityList = PairList<_BaseFunctionality>;
	using ComponentList = PairList<SComponent>;

	template<typename TType, typename TBaseType, typename TCreateFunc>
	inline TType& Add(PairList<TBaseType>& aList, TCreateFunc aCreateFunc, DependencyDestructor& aDestructor, const bool aExplicitlyAdded);

	template<typename TType, typename TBaseType>
	inline void Remove(PairList<TBaseType>& aList, const bool aExplicitlyRemoved);

	void Destroy();

	FunctionalityList myFunctionalities;
	ComponentList myComponents;
};

template<typename TType, typename TCreateFunc>
inline TType& Object::Add(TCreateFunc aCreateFunc, DependencyDestructor& aDestructor, const bool aExplicitlyAdded)
{
	if constexpr (std::is_base_of<SComponent, TType>::value)
	{
		return Add<TType>(myComponents, aCreateFunc, aDestructor, aExplicitlyAdded);
	}
	else if constexpr (std::is_base_of<_BaseFunctionality, TType>::value)
	{
		return Add<TType>(myFunctionalities, aCreateFunc, aDestructor, aExplicitlyAdded);
	}
	else
	{
		static_assert(false, "Type must inherit from SComponent or _BaseFunctionality!");
	}
}

template<typename TType>
inline void Object::Remove(const bool aExplicitlyRemoved)
{
	if constexpr (std::is_base_of<SComponent, TType>::value)
		Remove<TType>(myComponents, aExplicitlyRemoved);
	else if constexpr (std::is_base_of<_BaseFunctionality, TType>::value)
		Remove<TType>(myFunctionalities, aExplicitlyRemoved);
	else if constexpr (std::is_base_of<SystemAbstracted, TType>::value) {}
	else
		static_assert(false, "Type must inherit from SComponent or _BaseFunctionality!");
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
inline TType& Object::Add(PairList<TBaseType>& aList, TCreateFunc aCreateFunc, DependencyDestructor& aDestructor, const bool aExplicitlyAdded)
{
	const auto it = std::find_if(aList.begin(), aList.end(), [](const auto& aPair)
		{
			return aPair.first == typeid(TType);
		});

	if (it != aList.cend())
	{
		auto& counter = it->second;

		if (aExplicitlyAdded)
			counter.ExplicitlyAdded = true;
		else
			counter.RefCount++;

		return static_cast<TType&>(*counter.Reference);
	}

	auto& added = aCreateFunc(*this);

	aList.emplace_back(Pair<TBaseType>(typeid(TType), SRefCounter<TBaseType>(&added, &aDestructor, aExplicitlyAdded)));

	return added;
}

template<typename TType, typename TBaseType>
inline void Object::Remove(PairList<TBaseType>& aList, const bool aExplicitlyRemoved)
{
	const auto it = std::find_if(aList.begin(), aList.end(), [](const auto& aPair)
		{
			return aPair.first == typeid(TType);
		});

	if (it == aList.cend())
		return;

	auto& counter = it->second;

	assert(counter.RefCount != 0 || counter.ExplicitlyAdded);

	if (counter.RefCount > 0)
		counter.RefCount--;

	if (aExplicitlyRemoved)
		counter.ExplicitlyAdded = false;

	if (counter.RefCount > 0 || counter.ExplicitlyAdded)
		return;

	auto& ref = *counter.Reference;
	auto& destructor = *counter.Destructor;

	aList.erase(it);

	destructor.Destroy(*this, ref);
}
