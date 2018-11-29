#pragma once
#include <filesystem>
#include <fstream>
#include <string>


enum class EShaderType
{
	Vertex,
	Fragment
};

template<EShaderType ShaderType>
class Shader
{
public:
	Shader(const std::filesystem::path Path)
	{
		std::ifstream File{ Path };

		const size_t FileSize = std::filesystem::file_size(Path);

		const std::string ShaderString(FileSize, ' ');

		File.read(ShaderString.data(), FileSize);

		if constexpr (ShaderType == EShaderType::Vertex)
			ShaderHandle = glCreateShader(GL_VERTEX_SHADER);
		else if (ShaderType == EShaderType::Fragment)
			ShaderHandle = glCreateShader(GL_FRAGMENT_SHADER);

		const char* Raw = ShaderString.data();
		glShaderSource(ShaderHandle, 1, &Raw, nullptr);
		glCompileShader(ShaderHandle);

		i32  Success;
		char ErrorMessage[512];
		glGetShaderiv(ShaderHandle, GL_COMPILE_STATUS, &Success);

		if (!Success)
		{
			glGetShaderInfoLog(ShaderHandle, 512, nullptr, ErrorMessage);
			std::cout << "Shader compilation failed! \n" << ErrorMessage << std::endl;
		}
	}

	~Shader()
	{
		glDeleteShader(ShaderHandle);
	}

	u32 ShaderHandle;

	//don't allow copying and moving of shaders -- all shaders should be constructed in place to avoid errant behavior
	Shader(const Shader&) = delete;
	Shader(Shader&&) = delete;
	Shader& operator=(const Shader&) = delete;
	Shader& operator=(Shader&&) = delete;
};
