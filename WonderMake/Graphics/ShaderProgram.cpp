#include "stdafx.h"
#include "ShaderProgram.h"
#include <type_traits>
#include "VertexBuffer.h"
#include "Resources/ResourceManager.h"
#include "System/SystemPtr.h"
#include "Texture.h"

ShaderProgram::ShaderProgram(const std::filesystem::path& VertexShaderPath, const std::filesystem::path& FragmentShaderPath, const std::filesystem::path& GeometryShaderPath)
{
	SystemPtr<ResourceManager<Shader<EShaderType::Vertex>>> rmVertex;
	SystemPtr<ResourceManager<Shader<EShaderType::Fragment>>> rmFragment;
	
	myVertexShader = rmVertex->GetResource(VertexShaderPath);
	myFragmentShader = rmFragment->GetResource(FragmentShaderPath);

	if (!GeometryShaderPath.empty())
	{
		SystemPtr<ResourceManager<Shader<EShaderType::Geometry>>> rmGeometry;

		myGeometryShader.emplace(rmGeometry->GetResource(GeometryShaderPath));
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

		WmLog(TagError, "Error: Shader program linking failed: ", ErrorMessage);

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
