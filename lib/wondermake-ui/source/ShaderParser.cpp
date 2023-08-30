#include "ShaderParser.h"

#include "wondermake-base/Logger.h"
#include "wondermake-base/WmLogTags.h"

#include <algorithm>
#include <fstream>
#include <optional>
#include <string>
#include <unordered_set>
#include <vector>

namespace ShaderParser
{
	[[nodiscard]] std::optional<std::string> ParseShaderFileRecursive(const FilePath& aShaderPath, std::string&& aShaderCode, const bool aIsMain, const FilePath& aSearchPath, std::vector<FilePath>& aOutIncludedPaths, std::vector<std::pair<size_t, size_t>>& aOutCodeLines);

	ShaderParsedData ParseShader(const FilePath& aShaderPath, const FilePath& aSearchPath)
	{
		auto shaderFile = aShaderPath.GetFirstFileFromAllPaths();

		std::ifstream file{ shaderFile };

		const size_t fileSize = static_cast<size_t>(std::filesystem::file_size(shaderFile));

		std::string shaderString(fileSize, ' ');

		file.read(shaderString.data(), fileSize);

		std::optional<std::string>				allCode;
		std::vector<FilePath>					relatedFiles;
		std::vector<std::pair<size_t, size_t>>	codeLines;

		allCode = ParseShaderFileRecursive(aShaderPath, std::move(shaderString), true, aSearchPath, relatedFiles, codeLines);

		if (!allCode)
			WmLogError(TagWonderMake << TagWmOpenGL << "Failed to parse shader file with path: " << aShaderPath << '.');

		return ShaderParsedData(std::move(allCode), std::move(relatedFiles), std::move(codeLines));
	}
	
	std::optional<std::string> ParseShaderFileRecursive(const FilePath& aShaderPath, std::string&& aShaderCode, const bool aIsMain, const FilePath& aSearchPath, std::vector<FilePath>& aOutIncludedPaths, std::vector<std::pair<size_t, size_t>>& aOutCodeLines)
	{
		size_t fileIndex = aOutIncludedPaths.size();

		aOutIncludedPaths.emplace_back(aShaderPath);

		if (aShaderCode.empty())
			return {};

		const std::string alwaysExtract = "//!";
		const std::string extractIfMain = "//?";
		
		auto shaderPath = aShaderPath.Path.string();

		size_t rootDirectoryEnd = shaderPath.find_last_of('/');

		if (rootDirectoryEnd == std::string::npos)
			rootDirectoryEnd = shaderPath.find_last_of('\\');

		std::string root_include_directory = rootDirectoryEnd != std::string::npos ? shaderPath.substr(0, rootDirectoryEnd + 1) : "";

		if (root_include_directory.empty())
		{
			WmLogError(TagWonderMake << TagWmOpenGL << "Failed to parse shader, could not find root directory of shader with path:" << aShaderPath << '.');

			return std::nullopt;
		}

		const std::string_view includePrefix = "#include ";

		std::string result;
		std::string_view shaderCodeView = aShaderCode;

		const auto getFilePath = [&aShaderPath, &aSearchPath, &root_include_directory](const std::filesystem::path& aRawPath) -> std::optional<FilePath>
		{
			FilePath includePath = FilePath(aShaderPath.Location, root_include_directory / aRawPath)
				.LexicallyNormal();

			auto realPath = includePath
				.GetFirstFileFromAllPaths();

			if (std::filesystem::exists(realPath))
				return includePath;

			includePath = FilePath(aSearchPath.Location, aSearchPath.Path / aRawPath)
				.LexicallyNormal();

			realPath = includePath
				.GetFirstFileFromAllPaths();

			if (std::filesystem::exists(realPath))
				return includePath;
			
			WmLogError(TagWonderMake << TagWmOpenGL << "Failed to parse shader, could not find file to include with path: " << includePath << '.');

			return std::nullopt;
		};
		const auto readAndParseFile = [&aSearchPath, &aOutIncludedPaths](const FilePath& aPath, std::vector<std::pair<size_t, size_t>>& aOutCodeLines) -> std::optional<std::string>
		{
			if (std::ranges::find(aOutIncludedPaths, aPath) != aOutIncludedPaths.end())
				return std::nullopt;

			auto realPath = aPath
				.GetFirstFileFromAllPaths();

			std::ifstream includedFileStream{ realPath };
			const size_t fileSize = static_cast<size_t>(std::filesystem::file_size(realPath));

			std::string includedShaderString(fileSize, ' ');

			includedFileStream.read(includedShaderString.data(), fileSize);

			return ParseShaderFileRecursive(aPath, std::move(includedShaderString), false, aSearchPath, aOutIncludedPaths, aOutCodeLines);
		};

		size_t appendLine = 0;
		size_t pos = shaderCodeView.find(includePrefix);
		size_t fileLine = 0;

		while (pos >= alwaysExtract.length() && pos != std::string::npos)
		{
			auto includeLineBegin = shaderCodeView.begin() + (pos - alwaysExtract.length());

			size_t includeLineEndPos = shaderCodeView.find('\n', pos);

			auto includeLineEnd = shaderCodeView.begin() + includeLineEndPos;

			const auto includeLineView		= std::string_view(includeLineBegin, includeLineEnd);
			const auto includePrefixView	= std::string_view(includeLineBegin, includeLineBegin + alwaysExtract.length());

			const auto prependView = shaderCodeView.substr(appendLine, includeLineEndPos - appendLine);

			size_t lineCount = std::ranges::count(prependView, '\n') + 1;

			for (size_t i = 0; i < lineCount; ++i)
				aOutCodeLines.emplace_back(std::make_pair(fileIndex, fileLine + i));

			fileLine += lineCount;

			result += prependView;

			const auto parseInclude = [&]()
			{
				if (includePrefixView != alwaysExtract
					&& (!aIsMain || includePrefixView != extractIfMain))
					return;

				auto fileIncludeBegin = std::ranges::find(includeLineView, '"');

				if (fileIncludeBegin == includeLineView.end())
					return;

				auto fileIncludeEnd = std::find(fileIncludeBegin + 1, includeLineView.end(), '"');

				if (fileIncludeEnd == includeLineView.end())
					return;

				std::string_view fileView = std::string_view(fileIncludeBegin + 1, fileIncludeEnd);

				auto path = getFilePath(fileView);

				if (!path)
					return;

				std::vector<std::pair<size_t, size_t>> codeLines;

				auto parsedFileResult = readAndParseFile(*path, codeLines);

				if (!parsedFileResult)
					return;

				size_t lastIndex = aOutCodeLines.size();

				aOutCodeLines.resize(lastIndex + codeLines.size(), { 0, 0 });

				std::copy(codeLines.begin(), codeLines.end(), aOutCodeLines.begin() + lastIndex);

				result += '\n';

				result += *parsedFileResult;

				result += '\n';

				return;
			};

			parseInclude();

			appendLine = includeLineEndPos + 1;

			pos = aShaderCode.find(includePrefix, appendLine);
		}

		const auto appendView = shaderCodeView.substr(appendLine, pos - appendLine);
		
		size_t lineCount = std::ranges::count(appendView, '\n') + 1;

		for (size_t i = 0; i < lineCount; ++i)
			aOutCodeLines.emplace_back(std::make_pair(fileIndex, fileLine + i));

		result += appendView;

		return result;
	}
}