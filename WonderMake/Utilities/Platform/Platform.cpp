#include "pch.h"
#include "Platform.h"
#include "WindowsPlatform.h"

std::string Platform::GetDateTime()
{
	if constexpr (CurrentPlatform == EPlatform::Windows)
	{
		return WindowsPlatform::GetDateTime();
	}
	else if constexpr (CurrentPlatform == EPlatform::Linux)
	{
		//no debuglogging here cause it would cause an infinite loop
		return "Linux not currently supported";
	}
}

static std::filesystem::path ApplicationRoot;
std::filesystem::path Platform::GetApplicationRoot()
{
	StaticInitializationTimeFunction<[]
	{
		if constexpr (CurrentPlatform == EPlatform::Windows)
		{
			ApplicationRoot = WindowsPlatform::GetApplicationRoot();
		}
		else if constexpr (CurrentPlatform == EPlatform::Linux)
		{
			ApplicationRoot = "nyet"; //Linux not currently supported :( 
		}
	}>();

	return ApplicationRoot;
}
