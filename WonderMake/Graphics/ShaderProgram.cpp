#include "stdafx.h"
#include "ShaderProgram.h"
#include <type_traits>
#include "VertexBuffer.h"
#include "../Resources/ResourceManager.h"
#include "Texture.h"

ShaderProgram::ShaderProgram(const std::filesystem::path& VertexShaderPath, const std::filesystem::path& FragmentShaderPath, const std::filesystem::path& GeometryShaderPath)
{
	ResourceManager<Shader<EShaderType::Vertex>>* VertexRM = ResourceManager<Shader<EShaderType::Vertex>>::Get();
	ResourceManager<Shader<EShaderType::Fragment>>* FragmentRM = ResourceManager<Shader<EShaderType::Fragment>>::Get();

	myVertexShader = VertexRM->GetResource(VertexShaderPath);
	myFragmentShader = FragmentRM->GetResource(FragmentShaderPath);

	if (!GeometryShaderPath.empty())
	{
		ResourceManager<Shader<EShaderType::Geometry>>* GeomRM = ResourceManager<Shader<EShaderType::Geometry>>::Get();

		myGeometryShader.emplace(GeomRM->GetResource(GeometryShaderPath));
	}

	myProgramHandle = glCreateProgram();

	glAttachShader(myProgramHandle, myVertexShader->ShaderHandle);
	glAttachShader(myProgramHandle, myFragmentShader->ShaderHandle);

	if (myGeometryShader)
	{
		glAttachShader(myProgramHandle, (*myGeometryShader)->ShaderHandle);
	}

	glLinkProgram(myProgramHandle);

	i32 Success;
	char ErrorMessage[512];
	glGetProgramiv(myProgramHandle, GL_LINK_STATUS, &Success);

	if (!Success)
	{
		glGetProgramInfoLog(myProgramHandle, 512, NULL, ErrorMessage);
		std::cout << "Error: Shader program linking failed\n" << ErrorMessage << std::endl;

		glDeleteProgram(myProgramHandle);
	}
}

ShaderProgram::~ShaderProgram()
{
	if (myProgramHandle)
		glDeleteProgram(myProgramHandle);
}

void ShaderProgram::Activate()
{
	if (myProgramHandle)
		glUseProgram(myProgramHandle);
}
