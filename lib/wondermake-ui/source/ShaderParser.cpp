#include "ShaderParser.h"

#include "wondermake-base/Logger.h"
#include "wondermake-base/WmLogTags.h"

#include <fstream>

namespace ShaderParser
{
	std::pair<std::optional<std::string>, std::unordered_set<FilePath>> ParseShader(const FilePath& aShaderPath)
	{
		std::unordered_set<FilePath> includedFiles;
		auto shaderFile = aShaderPath.GetFirstFileFromAllPaths();

		std::ifstream file{ shaderFile };

		const size_t fileSize = static_cast<size_t>(std::filesystem::file_size(shaderFile));

		std::string shaderString(fileSize, ' ');

		file.read(shaderString.data(), fileSize);
		
		std::optional<std::string> parseResult = ParseShaderFileRecursive(aShaderPath, std::move(shaderString), true, includedFiles);

		if (!parseResult)
			WmLogError(TagWonderMake << TagWmOpenGL << "Failed to parse shader file with path: " << aShaderPath << '.');

		return std::make_pair(parseResult, std::move(includedFiles));
	}

	std::optional<std::string> ParseShaderFileRecursive(const FilePath& aShaderPath, std::string&& aShaderCode, const bool aIsMain, std::unordered_set<FilePath>& aOutIncludedPaths)
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

		auto shaderPath = aShaderPath.GetFirstFileFromAllPaths().string();

		size_t rootDirectoryEnd = shaderPath.find_last_of('/');

		if (rootDirectoryEnd == std::string::npos)
			rootDirectoryEnd = shaderPath.find_last_of('\\');

		std::string root_include_directory = rootDirectoryEnd != std::string::npos ? shaderPath.substr(0, rootDirectoryEnd + 1) : "";

		if (root_include_directory.empty())
		{
			WmLogError(TagWonderMake << TagWmOpenGL << "Failed to parse shader, could not find root directory of shader with path:" << aShaderPath << '.');

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
					std::filesystem::path rawPath = root_include_directory + aShaderCode.substr(startOfIncludeFilename + 1, endOfIncludeFilename - startOfIncludeFilename - 1);
					FilePath includePath;

					if (rawPath.is_absolute())
						includePath = FilePath::Resolve(rawPath);
					else
						includePath = FilePath(aShaderPath.Location, rawPath)
							.LexicallyNormal();

					const auto insertionResult = aOutIncludedPaths.insert(FilePath::Resolve(includePath));

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