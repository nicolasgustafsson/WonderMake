#pragma once

#include "Utilities/RestrictTypes.h"

struct SComponent : public NonCopyable
{
public:
	SComponent() = default;
	virtual ~SComponent() = default;
};

