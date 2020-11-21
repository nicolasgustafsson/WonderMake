#pragma once
#include <json/json.hpp>
#include <filesystem>

namespace std::filesystem
{
	inline void to_json(json& aJson, const path& aPath) {
		aJson = json{ {"path", aPath.wstring()} };
	}

	inline void from_json(const json& aJson, path& aPath) {
		wstring pathString;
		aJson.at("path").get_to(pathString);

		aPath = pathString;
	}

}
