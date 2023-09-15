#pragma once

#include "wondermake-base/ComponentSystem.h"

#include "wondermake-utility/RestrictTypes.h"

#define WM_REGISTER_COMPONENT(aComponent) WM_REGISTER_COMPONENT_SYSTEM(aComponent)

template<typename TComponent>
concept CNamedComponent = requires()
{
	{ TComponent::TypeName() } -> std::convertible_to<std::string_view>;
};

struct SComponent
	: public NonCopyable
{};

template<typename TSystemTraits>
struct STraitedComponent
	: public SComponent
{
	using SystemTraits = TSystemTraits;
};
