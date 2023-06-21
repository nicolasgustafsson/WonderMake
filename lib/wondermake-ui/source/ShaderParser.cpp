#include "wondermake-ui/ShaderParser.h"

#include "wondermake-base/Logger.h"
#include "wondermake-base/WmLogTags.h"

#include <fstream>

namespace ShaderParser
{
	std::optional<std::string> ParseShader(const std::filesystem::path& aShaderPath)
	{
		std::unordered_set<std::string> includedFiles;

		std::ifstream file{ aShaderPath };

		const size_t fileSize = static_cast<size_t>(std::filesystem::file_size(aShaderPath));

		std::string shaderString(fileSize, ' ');

		file.read(shaderString.data(), fileSize);

		std::optional<std::string> parseResult = ParseShaderFileRecursive(aShaderPath, std::move(shaderString), true, includedFiles);

		if (!parseResult)
			WmLogError(TagWonderMake << TagWmOpenGL << "Failed to parse shader file with path: " << aShaderPath.string() << '.');

		return parseResult;
	}

	std::optional<std::string> ParseShaderFileRecursive(const std::filesystem::path& aShaderPath, std::string&& aShaderCode, const bool aIsMain, std::unordered_set<std::string>& aOutIncludedPaths)
	{
		if (aShaderCode.empty())
			return {};

		std::optional<std::string> result("");

		const std::string alwaysExtract = "//!";
		const std::string extractIfMain = "//?";

		size_t pos = aShaderCode.find(alwaysExtract);

		while (pos != std::string::npos)
		{
			aShaderCode = aShaderCode.erase(pos, alwaysExtract.length());
			pos = aShaderCode.find(alwaysExtract);
		}

		if (aIsMain)
		{
			pos = aShaderCode.find(extractIfMain);

			while (pos != std::string::npos)
			{
				aShaderCode = aShaderCode.erase(pos, extractIfMain.length());
				pos = aShaderCode.find(extractIfMain);
			}
		}

		size_t rootDirectoryEnd = aShaderPath.string().find_last_of('/');

		if (rootDirectoryEnd == std::string::npos)
			rootDirectoryEnd = aShaderPath.string().find_last_of('\\');

		std::string root_include_directory = rootDirectoryEnd != std::string::npos ? aShaderPath.string().substr(0, rootDirectoryEnd + 1) : "";

		if (root_include_directory.empty())
		{
			WmLogError(TagWonderMake << TagWmOpenGL << "Failed to parse shader, could not find root directory of shader with path:" << aShaderPath.string() << '.');
			return {};
		}

		const char* includePrefix = "#include ";

		pos = aShaderCode.find(includePrefix);

		while (pos != std::string::npos)
		{
			const size_t startOfIncludeFilename = aShaderCode.find_first_of('"', pos);
			if (startOfIncludeFilename != std::string::npos)
			{
				const size_t endOfIncludeFilename = aShaderCode.find_first_of('"', startOfIncludeFilename + 1);
				if (endOfIncludeFilename != std::string::npos)
				{
					const std::string includePath = root_include_directory + aShaderCode.substr(startOfIncludeFilename + 1, endOfIncludeFilename - startOfIncludeFilename - 1);
					const auto insertionResult = aOutIncludedPaths.insert(includePath);

					if (insertionResult.second)
					{

						if (!std::filesystem::exists(includePath))
						{
							WmLogError(TagWonderMake << TagWmOpenGL << "Failed to parse shader, could not find file to include with path: " << includePath << '.');
							return {};
						}
						std::ifstream includedFileStream{ includePath };

						const size_t fileSize = static_cast<size_t>(std::filesystem::file_size(includePath));

						std::string includedShaderString(fileSize, ' ');

						includedFileStream.read(includedShaderString.data(), fileSize);

						std::optional<std::string> includedResult = ParseShaderFileRecursive(includePath, std::move(includedShaderString), false, aOutIncludedPaths);

						if (!includedResult)
						{
							WmLogError(TagWonderMake << TagWmOpenGL << "Failed to parse shader, could not parse included file with path: " << includePath << '.');
							return {};
						}

						(*result) += aShaderCode.substr(0, pos);
						(*result) += *includedResult;
					}

					aShaderCode = aShaderCode.erase(0, endOfIncludeFilename + 1);
				}
			}

			pos = aShaderCode.find(includePrefix);
		}
		(*result) += aShaderCode;

		return result;
	}
}