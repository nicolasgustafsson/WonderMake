#pragma once

#include <string>
#include <thread>

namespace WindowsPlatform
{
	std::string GetDateTime();

	std::filesystem::path GetApplicationRoot();

	void SetThreadName(const std::string& aThreadName);

	void SetThreadName(std::thread& aThread, const std::string& aThreadName);
}
