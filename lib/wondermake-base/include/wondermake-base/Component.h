#pragma once

#include "wondermake-base/ComponentSystem.h"

#include "wondermake-utility/RestrictTypes.h"

#define WM_REGISTER_COMPONENT(aComponent) WM_REGISTER_COMPONENT_SYSTEM(aComponent)

struct SComponent : public NonCopyable
{
public:
	SComponent() = default;
	virtual ~SComponent() = default;
};
