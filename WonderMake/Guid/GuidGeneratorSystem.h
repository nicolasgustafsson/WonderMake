#pragma once

#include "WonderMakeEngine/System.h"

#include "Guid/Guid.h"

class GuidGeneratorSystem
	: public SystemAbstracted
{
public:
	virtual ~GuidGeneratorSystem() = default;

	virtual std::optional<Guid> GenerateNew() = 0;

protected:
	GuidGeneratorSystem() noexcept = default;

};
