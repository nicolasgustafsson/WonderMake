#pragma once

#include "Utilities/RestrictTypes.h"

#define REGISTER_COMPONENT(aComponent)

struct SComponent : public NonCopyable
{
public:
	SComponent() = default;
	virtual ~SComponent() = default;
};
