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

ShaderProgram::ShaderProgram(ShaderProgram&& aOther) noexcept
{
	*this = std::move(aOther);
}

ShaderProgram& ShaderProgram::operator=(ShaderProgram&& aOther) noexcept
{
	myProgramHandle = std::move (aOther.myProgramHandle);
	myUniformSetters = std::move(aOther.myUniformSetters);
	myFragmentShader = std::move(aOther.myFragmentShader);
	myVertexShader = std::move(aOther.myVertexShader);
	myGeometryShader = std::move(aOther.myGeometryShader);

	if (aOther.myProgramHandle)
		aOther.myProgramHandle.reset();

	return *this;
}

void ShaderProgram::SetUniforms()
{
	if (!Activate())
		return;

	for(auto&& uniformSetter : myUniformSetters)
	{
		uniformSetter.second();
	}
}

void ShaderProgram::Create()
{
	SystemPtr<OpenGLFacade> openGL;
	myProgramHandle = openGL->CreateShaderProgram();

	openGL->AttachShaderToProgram(*myProgramHandle, myVertexShader->myShaderHandle);
	openGL->AttachShaderToProgram(*myProgramHandle, myFragmentShader->myShaderHandle);

	if (myGeometryShader)
	{
		openGL->AttachShaderToProgram(*myProgramHandle, (*myGeometryShader)->myShaderHandle);
	}

	openGL->LinkShaderProgram(*myProgramHandle);

	const i32 success = openGL->GetShaderProgramParameter(*myProgramHandle, GL_LINK_STATUS);

	if (!success)
	{
		const std::string errorMessage = openGL->GetShaderProgramInfoLog(*myProgramHandle);

		WmLog(TagError, "Error: Shader program linking failed: ", errorMessage);

		openGL->DeleteShaderProgram(*myProgramHandle);
		myProgramHandle.reset();
	}
}

ShaderProgram::~ShaderProgram()
{
	if (myProgramHandle)
	{
		Destroy();
	}
}

void ShaderProgram::SetShader(const EShaderType aShaderType, const std::filesystem::path& aShaderPath)
{
	switch(aShaderType)
	{
		case EShaderType::Vertex:
		{
			SystemPtr<ResourceSystem<Shader<EShaderType::Vertex>>> rmVertex;
			myVertexShader = rmVertex->GetResource(aShaderPath);
			break;
		}
		case EShaderType::Fragment:
		{
			SystemPtr<ResourceSystem<Shader<EShaderType::Fragment>>> rmFragment;
			myFragmentShader = rmFragment->GetResource(aShaderPath);
			break;
		}
		case EShaderType::Geometry:
		{
			SystemPtr<ResourceSystem<Shader<EShaderType::Geometry>>> rmGeometry;
			myGeometryShader = rmGeometry->GetResource(aShaderPath);
			break;
		}
	}

	Recreate();
}

void ShaderProgram::Destroy()
{
	SystemPtr<OpenGLFacade> openGL;
	openGL->DeleteShaderProgram(*myProgramHandle);
	myProgramHandle.reset();
}

void ShaderProgram::Recreate()
{
	Destroy();
	Create();
	SetUniforms();
}

bool ShaderProgram::Activate()
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
		openGL->UseShaderProgram(*myProgramHandle);

		return true;
	}

	return false;
}

bool ShaderProgram::CheckIfUpToDate()
{
	if (myFragmentShader.UpdateGeneration() == EResourceGenerationResult::NewGeneration)
		return false;
	if (myVertexShader.UpdateGeneration() == EResourceGenerationResult::NewGeneration)
		return false;
	if (myGeometryShader && myGeometryShader->UpdateGeneration() == EResourceGenerationResult::NewGeneration)
		return false;

	return true;
}
