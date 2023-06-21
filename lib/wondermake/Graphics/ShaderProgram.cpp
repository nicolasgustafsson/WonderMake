#include "ShaderProgram.h"

#include "Resources/ResourceSystem.h"
#include "Texture.h"

#include "wondermake-ui/OpenGLFacade.h"
#include "wondermake-ui/VertexBuffer.h"

#include "wondermake-base/Logger.h"
#include "wondermake-base/SystemPtr.h"
#include "wondermake-base/WmLogTags.h"

#include <type_traits>

ShaderProgram::ShaderProgram(
	ResourceSystem<Shader<EShaderType::Vertex>>& aVsSystem, ResourceSystem<Shader<EShaderType::Fragment>>& aFsSystem, ResourceSystem<Shader<EShaderType::Geometry>>& aGsSystem,
	const std::filesystem::path& VertexShaderPath, const std::filesystem::path& FragmentShaderPath, const std::filesystem::path& GeometryShaderPath)
{
	myVertexShader = aVsSystem.GetResource(VertexShaderPath);
	myFragmentShader = aFsSystem.GetResource(FragmentShaderPath);

	if (!GeometryShaderPath.empty())
	{
		myGeometryShader.emplace(aGsSystem.GetResource(GeometryShaderPath));
	}

	Create();
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

		WmLogError(TagWonderMake << TagWmOpenGL << "Shader program linking failed: {" << errorMessage << "}.");

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
