#pragma once

#include "wondermake-base/ObjectDependencyDestructor.h"

#include "wondermake-utility/Typedefs.h"

#include <cassert>
#include <memory>
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
	inline TType& Add(TCreateFunc aCreateFunc, std::shared_ptr<ObjectDependencyDestructor> aDestructor, const bool aExplicitlyAdded = true);

	// Will not remove the type if existing functionalities depend on it.
	template<typename TType>
	inline void Remove(const bool aExplicitlyRemoved = true);

private:
	template<typename TType>
	struct SRefCounter
	{
		constexpr SRefCounter(TType* const aReference, std::shared_ptr<ObjectDependencyDestructor>&& aDestructor, const bool aExplicitlyAdded) noexcept
			: Reference(aReference)
			, Destructor(std::move(aDestructor))
			, RefCount(aExplicitlyAdded ? 0 : 1)
			, ExplicitlyAdded(aExplicitlyAdded)
		{}

		TType*										Reference = nullptr;
		std::shared_ptr<ObjectDependencyDestructor>	Destructor;
		i32											RefCount = 0;
		bool										ExplicitlyAdded = false;
	};

	template<typename TType>
	using Pair = std::pair<std::type_index, SRefCounter<TType>>;
	template<typename TType>
	using PairList = std::vector<Pair<TType>>;

	using FunctionalityList = PairList<_BaseFunctionality>;
	using ComponentList = PairList<SComponent>;

	template<typename TType, typename TBaseType, typename TCreateFunc>
	inline TType& Add(PairList<TBaseType>& aList, TCreateFunc aCreateFunc, std::shared_ptr<ObjectDependencyDestructor>&& aDestructor, const bool aExplicitlyAdded);

	template<typename TType, typename TBaseType>
	inline void Remove(PairList<TBaseType>& aList, const bool aExplicitlyRemoved);

	void Destroy();

	FunctionalityList myFunctionalities;
	ComponentList myComponents;
};

template<typename TType, typename TCreateFunc>
inline TType& Object::Add(TCreateFunc aCreateFunc, std::shared_ptr<ObjectDependencyDestructor> aDestructor, const bool aExplicitlyAdded)
{
	static_assert(std::is_base_of_v<SComponent, TType> || std::is_base_of_v<_BaseFunctionality, TType>, "Type must inherit from SComponent or _BaseFunctionality!");

	if constexpr (std::is_base_of<SComponent, TType>::value)
		return Add<TType>(myComponents, aCreateFunc, std::move(aDestructor), aExplicitlyAdded);
	else if constexpr (std::is_base_of<_BaseFunctionality, TType>::value)
		return Add<TType>(myFunctionalities, aCreateFunc, std::move(aDestructor), aExplicitlyAdded);
}

template<typename TType>
inline void Object::Remove(const bool aExplicitlyRemoved)
{
	static_assert(std::is_base_of_v<SComponent, TType> || std::is_base_of_v<_BaseFunctionality, TType> || std::is_base_of_v<SystemAbstracted, TType>, "Type must inherit from SComponent, _BaseFunctionality, or SystemAbstracted!");

	if constexpr (std::is_base_of<SComponent, TType>::value)
		Remove<TType>(myComponents, aExplicitlyRemoved);
	else if constexpr (std::is_base_of<_BaseFunctionality, TType>::value)
		Remove<TType>(myFunctionalities, aExplicitlyRemoved);
}

template<typename TType, typename TBaseType, typename TCreateFunc>
inline TType& Object::Add(PairList<TBaseType>& aList, TCreateFunc aCreateFunc, std::shared_ptr<ObjectDependencyDestructor>&& aDestructor, const bool aExplicitlyAdded)
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

	auto refCounter = SRefCounter<TBaseType>(&added, std::move(aDestructor), aExplicitlyAdded);

	auto pair = Pair<TBaseType>(typeid(TType), std::move(refCounter));

	aList.emplace_back(std::move(pair));

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
	auto destructor = std::move(counter.Destructor);

	aList.erase(it);

	if (destructor)
		destructor->Destroy(*this, ref);
}
