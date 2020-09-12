#pragma once

#include "Utilities/RestrictTypes.h"

#include "Utilities/Serialization.h"

#define REGISTER_COMPONENT(aComponent) REGISTER_SERIALIZABLE(aComponent)

struct SComponent : public NonCopyable
{
public:
	SComponent() = default;
	virtual ~SComponent() = default;
};
