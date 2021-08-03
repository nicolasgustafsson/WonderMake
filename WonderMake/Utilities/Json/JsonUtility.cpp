#include "pch.h"
#include "JsonUtility.h"

namespace JsonUtility
{
	std::optional<nlohmann::json> LoadJson(const std::filesystem::path& aPath)
	{
		std::ifstream file(aPath, std::fstream::app);

		std::string fileContents((std::istreambuf_iterator<char>(file)),
			(std::istreambuf_iterator<char>()));

		std::optional<nlohmann::json> jsonFile{};

		if (json::accept(fileContents))
		{
			jsonFile = json::parse(fileContents);
		}

		return jsonFile;
	}
}