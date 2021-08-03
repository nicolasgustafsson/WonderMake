#pragma once
#include <fstream>
#include <optional>
#include <json/json.hpp>

namespace JsonUtility
{
	std::optional<nlohmann::json> LoadJson(const std::filesystem::path& aPath);
}
