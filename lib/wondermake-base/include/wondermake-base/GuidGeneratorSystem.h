#pragma once

#include "wondermake-base/System.h"

#include "wondermake-utility/Guid.h"

class GuidGeneratorSystem
	: public SystemAbstracted
{
public:
	virtual ~GuidGeneratorSystem() = default;

	virtual std::optional<Guid> GenerateNew() = 0;

protected:
	GuidGeneratorSystem() noexcept = default;

};
