#pragma once
//[Nicos]: Shamefully stolen and repurposed with permission from: https://github.com/carlivandev/sic

#include "wondermake-utility/FilePath.h"

#include <filesystem>
#include <optional>
#include <string>
#include <unordered_set>
#include <vector>

namespace ShaderParser
{
	[[nodiscard]] std::pair<std::optional<std::string>, std::unordered_set<FilePath>> ParseShader(const FilePath& in_shader_path);

	[[nodiscard]] std::optional<std::string> ParseShaderFileRecursive(const FilePath& aShaderPath, std::string&& aShaderCode, const bool aIsMain, std::unordered_set<FilePath>& aOutIncludedPaths);
};

