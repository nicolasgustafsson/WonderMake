#pragma once

#include "Guid/GuidGeneratorSystem.h"

class WindowsGuidGeneratorSystem
	: public GuidGeneratorSystem
{
public:
	std::optional<Guid> GenerateNew() override;

};
