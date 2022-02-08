#pragma once
//[Nicos]: Shamefully stolen and repurposed with permission from: https://github.com/carlivandev/sic

#include <string>
#include <unordered_set>
#include <optional>
#include <filesystem>

namespace ShaderParser
{
	[[nodiscard]] std::optional<std::string> ParseShader(const std::filesystem::path& in_shader_path);

	[[nodiscard]] std::optional<std::string> ParseShaderFileRecursive(const std::filesystem::path& aShaderPath, std::string&& aShaderCode, const bool aIsMain, std::unordered_set<std::string>& aOutIncludedPaths);
};

