#include "GenerateRegistry.h"

#include "json/json.hpp"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <regex>
#include <vector>

struct SFile
{
	std::vector<std::string> myObjectNames;
	std::filesystem::path myIncludePath;
};

std::ofstream CreateFile(
	const std::filesystem::path& aFilePath)
{
	std::ofstream fileStream(aFilePath);
	if (!fileStream)
	{
		std::cout << "Error: Failed to open file for writing; " << aFilePath << ".\n";

		return fileStream;
	}

	fileStream << " // This is a generated file, if you change it, you're going to have a bad time.\n\n";

	return fileStream;
}

bool GenerateHeaderFile(
	const std::filesystem::path& aOutputDir,
	const SFile& aFile)
{
	const std::filesystem::path fileName = aFile.myIncludePath.filename().stem();

	std::filesystem::path filePath = aOutputDir / std::filesystem::path("Register_").concat(fileName.string());

	filePath.replace_extension(".h");

	std::ofstream fileStream = CreateFile(filePath);

	if (!fileStream)
	{
		return false;
	}

	fileStream
		<< "#pragma once\n"
		<< "\n"
		<< "namespace Registry\n"
		<< "{\n"
		<< "\t\n";

	for (const auto& objectName : aFile.myObjectNames)
	{
		fileStream
			<< "\tvoid _Register" << objectName << "();\n"
			<< "\t\n";
	}

	fileStream
		<< "}\n";

	return true;
}

bool GenerateSourceFile(
	const std::filesystem::path& aOutputDir,
	const SFile& aFile)
{
	const std::filesystem::path fileName = aFile.myIncludePath.filename().stem();

	std::filesystem::path filePath = aOutputDir / std::filesystem::path("Register_").concat(fileName.string());

	filePath.replace_extension(".cpp");

	std::ofstream fileStream = CreateFile(filePath);

	if (!fileStream)
	{
		return false;
	}

	fileStream
		<< "#include \"pch.h\"\n"
		<< "\n"
		<< "#include \"Register_" << fileName.string() << ".h\"\n"
		<< "#include \"Serialization\\SerializationSystem.h\"\n"
		<< "#include " << aFile.myIncludePath << "\n"
		<< "\n"
		<< "namespace Registry\n"
		<< "{\n"
		<< "\t\n";

	for (const auto& objectName : aFile.myObjectNames)
	{
		fileStream
			<< "\tvoid _Register" << objectName << "()\n"
			<< "\t{\n"
			<< "\t\tSystemPtr<SerializationSystem>()->Register<" << objectName << ">();\n"
			<< "\t}\n"
			<< "\t\n";
	}

	fileStream
		<< "}\n";

	return true;
}

bool FindObjects(
	SFile& aFile)
{
	std::ifstream file(aFile.myIncludePath);

	if (!file)
	{
		std::cout << "Error: Unable to open file path; " << aFile.myIncludePath << ".\n";
		return false;
	}

	const std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	std::regex componentRegex("struct\\s*S\\S*Component");
	std::regex functionalityRegex("class\\s*\\S*Functionality");
	std::smatch stringMatch;

	const auto findObject = [&aFile](
		const std::smatch& aMatch)
	{
		for (const auto& match : aMatch)
		{
			const auto strMatch = match.str();

			const auto it = std::find_if(strMatch.rbegin(), strMatch.rend(), isspace);

			if (it == strMatch.rend())
			{
				std::cout << "Error: No whitespace found in regex match: \"" << strMatch << "\" in file " << aFile.myIncludePath << ".\n";
				return false;
			}

			const std::string name(it.base(), strMatch.end());

			aFile.myObjectNames.emplace_back(name);
		}

		return true;
	};

	std::regex_search(content, stringMatch, componentRegex, std::regex_constants::match_any);

	if (!findObject(stringMatch))
	{
		return false;
	}

	std::regex_search(content, stringMatch, functionalityRegex);

	if (!findObject(stringMatch))
	{
		return false;
	}

	return true;
}

bool ReadConfig(
	const json& aConfigJson,
	std::filesystem::path& aOutOutputDir,
	std::vector<std::filesystem::path>& aOutFiles,
	std::vector<std::filesystem::path>& aOutValidExtensions)
{
	{
		const auto it = aConfigJson.find("output_dir");

		if (it == aConfigJson.cend())
		{
			std::cout << "Error: 'output_dir' string missing from config.\n";

			return false;
		}

		if (!it->is_string())
		{
			std::cout << "Error: 'output_dir' must be a string.\n";

			return false;
		}

		aOutOutputDir = it->get<std::string>();

		if (std::filesystem::exists(aOutOutputDir)
			&& !std::filesystem::is_directory(aOutOutputDir))
		{
			std::cout << "Error: 'output_dir' must be a valid directory.\n";

			return false;
		}
	}

	{
		const auto it = aConfigJson.find("paths");

		if (it == aConfigJson.cend())
		{
			std::cout << "Error: 'paths' array missing from config.\n";

			return false;
		}

		if (!it->is_array())
		{
			std::cout << "Error: 'paths' must be an array.\n";

			return false;
		}

		aOutFiles.reserve(it->size());

		for (const auto& element : *it)
		{
			if (!element.is_string())
			{
				std::cout << "Error: Elements in 'paths' must be strings.\n";

				return false;
			}

			aOutFiles.emplace_back(element.get<std::string>());
		}
	}

	{
		const auto it = aConfigJson.find("valid_extensions");

		if (it == aConfigJson.cend())
		{
			std::cout << "Error: 'valid_extensions' array missing from config.\n";

			return false;
		}

		if (!it->is_array())
		{
			std::cout << "Error: 'paths' must be an array.\n";

			return false;
		}

		aOutValidExtensions.reserve(it->size());

		for (const auto& element : *it)
		{
			if (!element.is_string())
			{
				std::cout << "Error: Elements in 'valid_extensions' must be strings.\n";

				return false;
			}

			aOutValidExtensions.emplace_back(element.get<std::string>());
		}
	}

	return true;
}

bool GenerateRegistry(
	const std::filesystem::path& aConfigFile)
{
	if (!std::filesystem::exists(aConfigFile)
		|| !std::filesystem::is_regular_file(aConfigFile))
	{
		std::cout << "Error: Config file missing; " << aConfigFile << ".\n";

		return false;
	}

	std::ifstream configFile(aConfigFile);
	json configJson;
	std::filesystem::path outputDir;
	std::vector<std::filesystem::path> pathList;
	std::vector<std::filesystem::path> validExtensions;

	configFile >> configJson;

	if (!ReadConfig(configJson, outputDir, pathList, validExtensions))
	{
		return false;
	}

	if (!std::filesystem::exists(outputDir))
	{
		std::filesystem::create_directories(outputDir);
	}

	std::vector<SFile> files;

	for (size_t i = 0; i < pathList.size(); ++i)
	{
		const auto& path = pathList[i];

		if (std::filesystem::is_directory(path))
		{
			for (const auto& entry : std::filesystem::directory_iterator(path))
			{
				if (std::filesystem::is_directory(entry)
					|| std::find(validExtensions.cbegin(), validExtensions.cend(), entry.path().extension()) != validExtensions.cend())
				{
					pathList.emplace_back(entry.path());
				}
			}

			continue;
		}

		if (std::filesystem::is_regular_file(path))
		{
			SFile file;

			file.myIncludePath = path;

			if (!FindObjects(file))
			{
				return false;
			}

			if (file.myObjectNames.empty())
			{
				continue;
			}

			files.emplace_back(std::move(file));

			continue;
		}

		return false;
	}

	for (const auto& file : files)
	{
		if (!GenerateHeaderFile(outputDir, file)
			|| !GenerateSourceFile(outputDir, file))
		{
			return false;
		}
	}

	return true;
}
