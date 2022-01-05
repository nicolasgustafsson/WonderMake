#pragma once
#include "Graphics/Shader.h"
#include "Resources/ResourceProxy.h"
#include "WonderMakeBase/Vector.h"

#include <filesystem>
#include <optional>
#include "OpenGLFacade.h"

template<typename TResource>
class ResourceSystem;

template class ResourceSystem<Shader<EShaderType::Vertex>>;
template class ResourceSystem<Shader<EShaderType::Fragment>>;
template class ResourceSystem<Shader<EShaderType::Geometry>>;

class ShaderProgram : NonCopyable
{
public:
	ShaderProgram() = delete;
	ShaderProgram(
		ResourceSystem<Shader<EShaderType::Vertex>>& aVsSystem, ResourceSystem<Shader<EShaderType::Fragment>>& aFsSystem, ResourceSystem<Shader<EShaderType::Geometry>>& aGsSystem,
		const std::filesystem::path& aVertexShaderPath, const std::filesystem::path& aFragmentShaderPath, const std::filesystem::path& aGeometryShaderPath = "");

	~ShaderProgram();

	bool Activate();

	template<typename TProperty>
	void SetProperty(std::string_view aName, TProperty aProperty)
	{
		if (!Activate())
			return;

		SystemPtr<OpenGLFacade> openGL;

		const i32 location = openGL->GetUniformVariableLocation(*myProgramHandle, aName.data());

		openGL->SetUniformVariable(location, aProperty);
	}	

private:
	void Create();
	void Destroy();
	void Recreate();
	bool CheckIfUpToDate();

	std::optional<u32> myProgramHandle;

	ResourceProxy<Shader<EShaderType::Vertex>> myVertexShader;
	ResourceProxy<Shader<EShaderType::Fragment>> myFragmentShader;
	std::optional<ResourceProxy<Shader<EShaderType::Geometry>>> myGeometryShader;
};

