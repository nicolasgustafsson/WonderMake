#pragma once

#include "Components/ComponentSystem.h"

#include "Utilities/RestrictTypes.h"

#define REGISTER_COMPONENT(aComponent) REGISTER_COMPONENT_SYSTEM(aComponent)

struct SComponent : public NonCopyable
{
public:
	SComponent() = default;
	virtual ~SComponent() = default;
};
