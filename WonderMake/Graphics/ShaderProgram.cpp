#include "pch.h"
#include "ShaderProgram.h"
#include <type_traits>
#include "VertexBuffer.h"
#include "Resources/ResourceSystem.h"
#include "System/SystemPtr.h"
#include "Texture.h"
#include "OpenGLFacade.h"

ShaderProgram::ShaderProgram(const std::filesystem::path& VertexShaderPath, const std::filesystem::path& FragmentShaderPath, const std::filesystem::path& GeometryShaderPath)
{
	SystemPtr<ResourceSystem<Shader<EShaderType::Vertex>>> rmVertex;
	SystemPtr<ResourceSystem<Shader<EShaderType::Fragment>>> rmFragment;

	myVertexShader = rmVertex->GetResource(VertexShaderPath);
	myFragmentShader = rmFragment->GetResource(FragmentShaderPath);

	if (!GeometryShaderPath.empty())
	{
		SystemPtr<ResourceSystem<Shader<EShaderType::Geometry>>> rmGeometry;
		
		myGeometryShader.emplace(rmGeometry->GetResource(GeometryShaderPath));
	}

	Create();
}

void ShaderProgram::Create()
{
	SystemPtr<OpenGLFacade> openGL;
	myProgramHandle = openGL->CreateShaderProgram();

	openGL->AttachShaderToProgram(myProgramHandle, myVertexShader->myShaderHandle);
	openGL->AttachShaderToProgram(myProgramHandle, myFragmentShader->myShaderHandle);

	if (myGeometryShader)
	{
		openGL->AttachShaderToProgram(myProgramHandle, (*myGeometryShader)->myShaderHandle);
	}

	openGL->LinkShaderProgram(myProgramHandle);

	const i32 success = openGL->GetShaderProgramParameter(myProgramHandle, GL_LINK_STATUS);

	if (!success)
	{
		const std::string errorMessage = openGL->GetShaderProgramInfoLog(myProgramHandle);

		WmLog(TagError, "Error: Shader program linking failed: ", errorMessage);

		openGL->DeleteShaderProgram(myProgramHandle);
		myProgramHandle = 0;
	}
}

ShaderProgram::~ShaderProgram()
{
	if (myProgramHandle)
	{
		Destroy();
	}
}

void ShaderProgram::Destroy()
{
	SystemPtr<OpenGLFacade> openGL;
	openGL->DeleteShaderProgram(myProgramHandle);
	myProgramHandle = 0;
}

void ShaderProgram::Recreate()
{
	Destroy();
	Create();
}

void ShaderProgram::Activate()
{
	if constexpr (Constants::EnableAssetHotReload)
	{
		if (!CheckIfUpToDate())
		{
			Recreate();
		}
	}

	if (myProgramHandle)
	{
		SystemPtr<OpenGLFacade> openGL;
		openGL->UseShaderProgram(myProgramHandle);
	}
}

bool ShaderProgram::CheckIfUpToDate()
{
	if (myFragmentShader.CheckGeneration() == EResourceGenerationResult::NewGeneration)
		return false;
	if (myVertexShader.CheckGeneration() == EResourceGenerationResult::NewGeneration)
		return false;
	if (myGeometryShader && myGeometryShader->CheckGeneration() == EResourceGenerationResult::NewGeneration)
		return false;

	return true;
}
