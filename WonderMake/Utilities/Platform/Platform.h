#pragma once

enum class EPlatform
{
	Windows,
	Linux,
	//Mac LMAO
};

constexpr const static inline EPlatform CurrentPlatform = EPlatform::Windows;

namespace Platform
{
	std::string GetDateTime();

	std::filesystem::path GetApplicationRoot();
}