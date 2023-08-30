#pragma once

#include "wondermake-utility/FilePath.h"

#include <optional>
#include <string>
#include <vector>

class ShaderParsedData
{
public:
	struct SShaderCodeLine
	{
		FilePath	Path;
		size_t		Line = 0;
	};

	inline ShaderParsedData(
		std::optional<std::string>				aAllCode,
		std::vector<FilePath>					aRelatedFiles,
		std::vector<std::pair<size_t, size_t>>	aCodeLines)
		: myAllCode(std::move(aAllCode))
		, myRelatedFiles(std::move(aRelatedFiles))
		, myCodeLines(std::move(aCodeLines))
	{}

	[[nodiscard]] inline const std::optional<std::string>& AllCode() const noexcept
	{
		return myAllCode;
	}
	[[nodiscard]] inline const auto& RelatedFiles() const noexcept
	{
		return myRelatedFiles;
	}

	[[nodiscard]] SShaderCodeLine GetShaderLineInfo(size_t aLine) const noexcept;

private:
	std::optional<std::string>				myAllCode;
	std::vector<FilePath>					myRelatedFiles;
	std::vector<std::pair<size_t, size_t>>	myCodeLines;

};
