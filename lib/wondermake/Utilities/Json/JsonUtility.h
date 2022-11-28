#pragma once

#include <json/json.hpp>

#include <filesystem>
#include <optional>

namespace JsonUtility
{
	std::optional<nlohmann::json> LoadJson(const std::filesystem::path& aPath);
}
