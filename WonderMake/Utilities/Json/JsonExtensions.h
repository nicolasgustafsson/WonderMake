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

namespace std
{
	template<typename T>
	inline void to_json(json& aJson, const optional<T>& aOptional) 
	{
		if (aOptional)
			aJson = { {"hasValue", true}, {"value", *aOptional} };
		else
			aJson = { {"hasValue", false} };
	}

	template<typename T>
	inline void from_json(const json& aJson, optional<T>& aOptional)
	{
		if (!aJson["hasValue"].get<bool>())
			return;
		
		aOptional = aJson["value"].get<T>();
	}
}
