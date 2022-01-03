#pragma once

#include <filesystem>

#include "Typedefs.h"

#include "Utilities/UniqueFunction.h"

namespace Engine
{
	//wroom wroom
	void Start(std::filesystem::path&& aProjectFolderNames, Closure&& aCallback);
};

