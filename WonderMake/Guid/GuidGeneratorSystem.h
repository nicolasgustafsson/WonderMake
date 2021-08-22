#pragma once

#include "System/System.h"

#include "Guid/Guid.h"

class GuidGeneratorSystem
	: public System<>
{
public:
	virtual ~GuidGeneratorSystem() = default;

	virtual std::optional<Guid> GenerateNew() = 0;

protected:
	GuidGeneratorSystem() noexcept = default;

};
