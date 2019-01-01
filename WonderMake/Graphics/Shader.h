#pragma once
#include <fstream>
#include <iostream>

#include "../Utilities/RestrictTypes.h"

enum class EShaderType
{
	Vertex,
	Fragment,
	Geometry
};

template<EShaderType ShaderType>
class Shader : NonCopyable
{
public:
	Shader(const std::filesystem::path Path)
	{
		std::ifstream File{ Path };

		const size_t FileSize = std::filesystem::file_size(Path);

		std::string ShaderString(FileSize, ' ');

		File.read(ShaderString.data(), FileSize);

		if constexpr (ShaderType == EShaderType::Vertex)
			ShaderHandle = glCreateShader(GL_VERTEX_SHADER);
		else if constexpr (ShaderType == EShaderType::Fragment)
			ShaderHandle = glCreateShader(GL_FRAGMENT_SHADER);
		else if constexpr (ShaderType == EShaderType::Geometry)
			ShaderHandle = glCreateShader(GL_GEOMETRY_SHADER);

		const char* Raw = ShaderString.data();
		glShaderSource(ShaderHandle, 1, &Raw, nullptr);
		glCompileShader(ShaderHandle);

		i32  Success;
		char ErrorMessage[512];
		glGetShaderiv(ShaderHandle, GL_COMPILE_STATUS, &Success);

		if (!Success)
		{
			glGetShaderInfoLog(ShaderHandle, 512, nullptr, ErrorMessage);
			WmLog(TagError, TagOpenGL, "Shader compilation failed: ", ErrorMessage);
		}
	}

	Shader(Shader&&) = default;
	Shader& operator=(Shader&&) = default;

	~Shader()
	{
		glDeleteShader(ShaderHandle);
	}

	u32 ShaderHandle;
};
