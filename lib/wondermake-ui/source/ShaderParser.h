#pragma once
//[Nicos]: Shamefully stolen and repurposed with permission from: https://github.com/carlivandev/sic
//[Kevin]: Shamelessly refurbished to better fit my needs without permission from: https://github.com/carlivandev/sic - Get got scrub

#include "wondermake-utility/FilePath.h"

#include <optional>
#include <string>
#include <unordered_set>

namespace ShaderParser
{
	[[nodiscard]] std::pair<std::optional<std::string>, std::unordered_set<FilePath>> ParseShader(const FilePath& aShaderPath, const FilePath& aSearchPath);
};

