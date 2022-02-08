#pragma once

#include "wondermake-base/GuidGeneratorSystem.h"

class WinPlatformSystem;

class WinGuidGeneratorSystem
	: public GuidGeneratorSystem
	, public SystemSub<
		Policy::Set<
			PAdd<WinPlatformSystem, PWrite>>>
{
public:
	std::optional<Guid> GenerateNew() override;

};
