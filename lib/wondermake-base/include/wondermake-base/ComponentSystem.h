#pragma once

#include "wondermake-base/Object.h"
#include "wondermake-base/ObjectDependencyDestructor.h"
#include "wondermake-base/System.h"

#include "wondermake-utility/plf_colony.h"
#include "wondermake-utility/SharedReference.h"

#include <memory>

class Object;

template<typename TComponent>
concept CTraitedComponent = requires()
{
	typename TComponent::SystemTraits;
};

namespace _Impl
{
	template<typename TComponent>
	struct SComponentSystemTraits
	{
		using Set = STrait::Set<>;
	};

	template<CTraitedComponent TComponent>
	struct SComponentSystemTraits<TComponent>
	{
		using Set = typename TComponent::SystemTraits;
	};
}

template<typename TComponent>
class ComponentSystem
	: public System<
		Policy::Set<>,
		typename _Impl::SComponentSystemTraits<TComponent>::Set>
	, public std::enable_shared_from_this<ComponentSystem<TComponent>>
{
public:
	inline ComponentSystem()
		: myDependencyDestructor([this](Object& /*aObject*/, auto* aComponent)
			{
				RemoveComponent(*static_cast<TComponent*>(aComponent));
			})
	{}

	inline TComponent& AddComponent(Object& aObject, const bool aExplicitlyAdded = true)
	{
		return aObject.Add<TComponent>([this](auto& /*aObject*/) -> auto&
		{
			return *myData.emplace();
		}, std::shared_ptr<ObjectDependencyDestructor>(this->shared_from_this(), &myDependencyDestructor), aExplicitlyAdded);
	}
	inline void RemoveComponent(TComponent& aComponent)
	{
		myData.erase(myData.get_iterator_from_pointer(&aComponent));
	}

	inline [[nodiscard]] bool IsEmpty() const noexcept
	{
		return myData.empty();
	}

private:
	plf::colony<TComponent>		myData;
	ObjectDependencyDestructor	myDependencyDestructor;
};

#define WM_REGISTER_COMPONENT_SYSTEM(aComponent) _WM_REGISTER_SYSTEM_IMPL(ComponentSystem<aComponent>, aComponent)
