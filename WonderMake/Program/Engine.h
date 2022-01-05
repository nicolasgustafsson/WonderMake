#pragma once

#include "WonderMakeBase/UniqueFunction.h"

#include "WonderMakeBase/Typedefs.h"

#include <filesystem>
#include <string>

namespace Engine
{
	struct Info
	{
		bool Headless = true;
		std::filesystem::path ProjectFolderNames;
	};

	struct Callbacks
	{
		Closure OnSetup;
	};

	//wroom wroom
	void Start(Info&& aInfo, Callbacks&& aCallbacks);
};

