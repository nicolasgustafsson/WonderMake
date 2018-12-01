#include "stdafx.h"
#include "ShaderProgram.h"
#include <type_traits>
#include "VertexBuffer.h"

ShaderProgram::ShaderProgram(const std::filesystem::path& VertexShaderPath, const std::filesystem::path& FragmentShaderPath, const std::filesystem::path& GeometryShaderPath)
	: myVertexShader(VertexShaderPath), myFragmentShader(FragmentShaderPath)
{
	myProgramHandle = glCreateProgram();

	if (!GeometryShaderPath.empty())
	{
		myGeometryShader.emplace(GeometryShaderPath);
	}

	glAttachShader(*myProgramHandle, myVertexShader.ShaderHandle);
	glAttachShader(*myProgramHandle, myFragmentShader.ShaderHandle);

	if (myGeometryShader)
	{
		glAttachShader(*myProgramHandle, myGeometryShader->ShaderHandle);
	}

	glLinkProgram(*myProgramHandle);

	i32 Success;
	char ErrorMessage[512];
	glGetProgramiv(*myProgramHandle, GL_LINK_STATUS, &Success);

	if (!Success)
	{
		glGetProgramInfoLog(*myProgramHandle, 512, NULL, ErrorMessage);
		std::cout << "Error: Shader program linking failed\n" << ErrorMessage << std::endl;

		glDeleteProgram(myProgramHandle.value());
		myProgramHandle.reset();
	}
}

ShaderProgram::~ShaderProgram()
{
	if (myProgramHandle)
		glDeleteProgram(*myProgramHandle);
}

void ShaderProgram::Activate()
{
	if (myProgramHandle)
		glUseProgram(*myProgramHandle);
}
