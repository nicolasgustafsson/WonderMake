#pragma once
#include <fstream>

#include "Resources/Resource.h"

#include "Utilities/RestrictTypes.h"

#include <glad/glad.h>
#include "OpenGLFacade.h"

enum class EShaderType
{
	Vertex,
	Fragment,
	Geometry
};

template<EShaderType ShaderType>
class Shader
	: public Resource
	, private NonCopyable
{
public:
	Shader(const std::filesystem::path Path)
	{
		SystemPtr<OpenGLFacade> openGL;
		std::ifstream file{ Path };

		const size_t fileSize = std::filesystem::file_size(Path);

		std::string shaderString(fileSize, ' ');

		file.read(shaderString.data(), fileSize);

		if constexpr (ShaderType == EShaderType::Vertex)
			myShaderHandle = openGL->CreateShader(GL_VERTEX_SHADER);
		else if constexpr (ShaderType == EShaderType::Fragment)
			myShaderHandle = openGL->CreateShader(GL_FRAGMENT_SHADER);
		else if constexpr (ShaderType == EShaderType::Geometry)
			myShaderHandle = openGL->CreateShader(GL_GEOMETRY_SHADER);

		const char* raw = shaderString.data();
		openGL->SetShaderSource(myShaderHandle, raw);
		openGL->CompileShader(myShaderHandle);

		const i32 compileWasSuccessful = openGL->GetShaderParameter(myShaderHandle, GL_COMPILE_STATUS);

		if (!compileWasSuccessful)
		{
			const std::string errorMessage = openGL->GetShaderInfoLog(myShaderHandle);
			WmLog(TagError, TagOpenGL, "Shader compilation failed: ", errorMessage);
		}
	}

	Shader(Shader&&) = default;
	Shader& operator=(Shader&&) = default;

	~Shader()
	{
		SystemPtr<OpenGLFacade> openGL;
		openGL->DeleteShader(myShaderHandle);
	}

	u32 myShaderHandle;
};
