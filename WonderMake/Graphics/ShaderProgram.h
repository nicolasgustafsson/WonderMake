#pragma once
#include "Graphics/Shader.h"
#include "Resources/ResourceProxy.h"
#include "Utilities/Vector.h"
#include "Utilities/Vector.h"
#include "Utilities/Vector.h"

#include <filesystem>
#include <optional>
#include "OpenGLFacade.h"

class ShaderProgram : NonCopyable
{
public:
	ShaderProgram() = delete;
	ShaderProgram(const std::filesystem::path& aVertexShaderPath, const std::filesystem::path& aFragmentShaderPath, const std::filesystem::path& aGeometryShaderPath = "");

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

