#pragma once

#include "wondermake-base/GuidGeneratorSystem.h"

class WinPlatformSystem;

class WinGuidGeneratorSystem
	: public GuidGeneratorSystem
	, public SystemSub<
		Policy::Set<
			PAdd<WinPlatformSystem, PWrite>>,
		STrait::Set<
			STWonderMake>>
{
public:
	std::optional<Guid> GenerateNew() override;

};
