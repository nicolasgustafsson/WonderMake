#include "GenerateRegistry.h"

#include "json/json.hpp"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <vector>

struct SLexedFile
{
	std::filesystem::path myIncludePath;
	std::filesystem::path myHeaderFile;
	std::filesystem::path mySourceFile;
	std::string myRegisterFuncName;
	std::vector<std::string> myObjectNames;
};

bool CompareFile(const std::filesystem::path& aFilePath, const std::string_view aContent)
{
	std::ifstream fileStream(aFilePath);
	std::stringstream sstr;

	sstr << fileStream.rdbuf();
	
	const auto fileContent = sstr.str();

	return aContent == fileContent;
}

bool WriteFile(const std::filesystem::path& aFilePath, const std::string_view aContent)
{
	if (CompareFile(aFilePath, aContent))
	{
		return true;
	}

	std::cout << "Info: File out of date, generating; " << aFilePath << ".\n";

	std::ofstream fileStream(aFilePath);

	if (!fileStream)
	{
		std::cout << "Error: Failed to open file for writing; " << aFilePath << ".\n";

		return false;
	}

	fileStream << aContent;

	fileStream.flush();
	fileStream.close();

	return true;
}

void FileTemplate(std::stringstream& aOutStream)
{
	aOutStream << " // This is a generated file, if you change it, you're going to have a bad time.\n\n";
}

bool GenerateHeaderFile(const std::filesystem::path& aOutputDir, const SLexedFile& aFile)
{
	std::stringstream sstr;

	FileTemplate(sstr);

	sstr
		<< "#pragma once\n"
		<< "\n"
		<< "namespace Registry\n"
		<< "{\n"
		<< "\tvoid " << aFile.myRegisterFuncName << "();\n"
		<< "}\n";

	const std::filesystem::path filePath = aOutputDir / aFile.myHeaderFile;
	const auto content = sstr.str();

	return WriteFile(filePath, content);
}

bool GenerateSourceFile(const std::filesystem::path& aOutputDir, const SLexedFile& aFile)
{
	std::stringstream sstr;

	FileTemplate(sstr);

	sstr
		<< "#include \"pch.h\"\n"
		<< "\n"
		<< "#include " << aFile.myHeaderFile << "\n"
		<< "#include \"Serialization\\SerializationSystem.h\"\n"
		<< "#include " << aFile.myIncludePath << "\n"
		<< "\n"
		<< "namespace Registry\n"
		<< "{\n"
		<< "\tvoid " << aFile.myRegisterFuncName << "()\n"
		<< "\t{\n";

	for (const auto& objectName : aFile.myObjectNames)
	{
		sstr
			<< "\t\tSystemPtr<SerializationSystem>()->Register<" << objectName << ">();\n";
	}

	sstr
		<< "\t}\n"
		<< "}\n";

	std::filesystem::path filePath = aOutputDir / aFile.mySourceFile;
	const auto content = sstr.str();

	filePath.replace_extension(".cpp");

	return WriteFile(filePath, content);
}

bool GenerateRegistryHeaderFile(const std::filesystem::path& aFilePath)
{
	std::stringstream sstr;

	FileTemplate(sstr);

	sstr
		<< "#pragma once\n"
		<< "\n"
		<< "namespace Registry\n"
		<< "{\n"
		<< "\tvoid Register();\n"
		<< "}\n";

	const auto content = sstr.str();

	return WriteFile(aFilePath, content);
}

bool GenerateRegistrySourceFile(const std::filesystem::path& aFilePath, const std::vector<SLexedFile>& aFiles)
{
	std::stringstream sstr;

	FileTemplate(sstr);

	sstr
		<< "#include \"pch.h\"\n"
		<< "\n";

	for (const auto& file : aFiles)
	{
		sstr
			<< "#include " << file.myHeaderFile << "\n";
	}

	sstr
		<< "\n"
		<< "namespace Registry\n"
		<< "{\n"
		<< "\tvoid Register()\n"
		<< "\t{\n";

	for (const auto& file : aFiles)
	{
		sstr
			<< "\t\t" << file.myRegisterFuncName << "();\n";
	}

	sstr
		<< "\t}\n"
		<< "}\n";

	const auto content = sstr.str();

	return WriteFile(aFilePath, content);
}

bool FindObjectsInFile(
	const std::filesystem::path& aFilePath,
	SLexedFile& aOutFile)
{
	std::ifstream file(aFilePath);

	if (!file)
	{
		std::cout << "Error: Unable to open file path; " << aFilePath << ".\n";
		return false;
	}

	const std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	std::regex componentRegex("struct\\s+[^_]S\\S+Component\\s*[^;]");
	std::regex functionalityRegex("class\\s+[^_]\\S+Functionality\\s*[^;]");
	std::smatch stringMatch;

	const auto findObject = [&aOutFile, &aFilePath](
		const std::smatch& aMatch)
	{
		for (const auto& match : aMatch)
		{
			const auto strMatch = match.str();

			auto startIt = std::find_if(strMatch.begin(), strMatch.end(), isspace);
			startIt = std::find_if(startIt, strMatch.end(), isalpha);

			if (startIt == strMatch.end())
			{
				std::cout << "Error: Start of object not found in regex match: \"" << strMatch << "\" in file " << aFilePath << ".\n";
				return false;
			}

			const auto endIt = std::find_if(startIt + 1, strMatch.end(), isspace);

			if (endIt == strMatch.end())
			{
				std::cout << "Error: End of object not found in regex match: \"" << strMatch << "\" in file " << aFilePath << ".\n";
				return false;
			}

			const std::string name(startIt, endIt);

			aOutFile.myObjectNames.emplace_back(name);
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

bool Lex(
	const std::filesystem::path& aFilePath,
	SLexedFile& aOutFile)
{
	const std::string fileName = aFilePath.filename().stem().string();

	aOutFile.myIncludePath = aFilePath;
	aOutFile.myHeaderFile = "Register_" + fileName + ".h";
	aOutFile.mySourceFile = "Register_" + fileName + ".cpp";
	aOutFile.myRegisterFuncName = "_Register" + fileName;

	return FindObjectsInFile(aFilePath, aOutFile);
}

bool ReadConfig(
	const json& aConfigJson,
	std::filesystem::path& aOutOutputDir,
	std::filesystem::path& aOutRegistryHeaderFile,
	std::filesystem::path& aOutRegistrySourceFile,
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
		const auto it = aConfigJson.find("output_registry_header_file");

		if (it == aConfigJson.cend())
		{
			std::cout << "Error: 'output_registry_header_file' string missing from config.\n";

			return false;
		}

		if (!it->is_string())
		{
			std::cout << "Error: 'output_registry_header_file' must be a string.\n";

			return false;
		}

		aOutRegistryHeaderFile = it->get<std::string>();
	}

	{
		const auto it = aConfigJson.find("output_registry_source_file");

		if (it == aConfigJson.cend())
		{
			std::cout << "Error: 'output_registry_source_file' string missing from config.\n";

			return false;
		}

		if (!it->is_string())
		{
			std::cout << "Error: 'output_registry_source_file' must be a string.\n";

			return false;
		}

		aOutRegistrySourceFile = it->get<std::string>();
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
	std::filesystem::path registryHeaderFile;
	std::filesystem::path registrySourceFile;
	std::vector<std::filesystem::path> pathList;
	std::vector<std::filesystem::path> validExtensions;

	configFile >> configJson;

	if (!ReadConfig(configJson, outputDir, registryHeaderFile, registrySourceFile, pathList, validExtensions))
	{
		return false;
	}

	if (!std::filesystem::exists(outputDir))
	{
		std::filesystem::create_directories(outputDir);
	}

	std::vector<SLexedFile> files;

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
			SLexedFile file;
			
			if (!Lex(path, file))
			{
				return false;
			}

			if (file.myObjectNames.empty())
			{
				continue;
			}

			files.emplace_back(file);

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

	if (!GenerateRegistryHeaderFile(registryHeaderFile)
		|| !GenerateRegistrySourceFile(registrySourceFile, files))
	{
		return false;
	}

	return true;
}
