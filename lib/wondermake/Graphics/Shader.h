#pragma once
#include <fstream>

#include "Resources/Resource.h"
#include "Resources/ResourceSystem.h"

#include "wondermake-base/Logger.h"
#include "wondermake-base/SystemPtr.h"
#include "wondermake-base/WmLogTags.h"

#include "wondermake-utility/RestrictTypes.h"

#include <glad/glad.h>
#include "OpenGLFacade.h"
#include "Graphics/ShaderParser.h"

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
	Shader(const std::filesystem::path& aPath)
	{
		SystemPtr<OpenGLFacade> openGL;
		
		std::optional<std::string> shaderString = ShaderParser::ParseShader(aPath);

		if (!shaderString)
		{
			WmLogError(TagWonderMake << TagWmOpenGL << "Shader preprocessing failed: " << aPath.string() << '.');
			return;
		}

		if constexpr (ShaderType == EShaderType::Vertex)
			myShaderHandle = openGL->CreateShader(GL_VERTEX_SHADER);
		else if constexpr (ShaderType == EShaderType::Fragment)
			myShaderHandle = openGL->CreateShader(GL_FRAGMENT_SHADER);
		else if constexpr (ShaderType == EShaderType::Geometry)
			myShaderHandle = openGL->CreateShader(GL_GEOMETRY_SHADER);

		const char* raw = shaderString->data();
		openGL->SetShaderSource(myShaderHandle, raw);
		openGL->CompileShader(myShaderHandle);

		const i32 compileWasSuccessful = openGL->GetShaderParameter(myShaderHandle, GL_COMPILE_STATUS);

		if (!compileWasSuccessful)
		{
			const std::string errorMessage = openGL->GetShaderInfoLog(myShaderHandle);
			WmLogError(TagWonderMake << TagWmOpenGL << "Shader compilation failed: {" << errorMessage << "}.");
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

template<>
inline [[nodiscard]] std::string GetResourceTypeName<Shader<EShaderType::Vertex>>()
{
	return "Vertex Shader";
}
template<>
inline [[nodiscard]] std::string GetResourceTypeName<Shader<EShaderType::Fragment>>()
{
	return "Fragment Shader";
}
template<>
inline [[nodiscard]] std::string GetResourceTypeName<Shader<EShaderType::Geometry>>()
{
	return "Geometry Shader";
}
