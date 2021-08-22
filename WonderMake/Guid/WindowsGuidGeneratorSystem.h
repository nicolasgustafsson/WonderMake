#pragma once

#include "Guid/GuidGeneratorSystem.h"

class PlatformWindowsSystem;

class WindowsGuidGeneratorSystem
	: public GuidGeneratorSystem
	, public SystemSub<
		Policy::Set<
			PAdd<PlatformWindowsSystem, PWrite>>>
{
public:
	std::optional<Guid> GenerateNew() override;

};
