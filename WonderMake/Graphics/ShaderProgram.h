#pragma once
#include "Graphics/Shader.h"
#include "Resources/ResourceProxy.h"
#include "Utilities/Vector.h"

#include <filesystem>
#include <optional>
#include "OpenGLFacade.h"
#include "Utilities/Container/Container.h"

class ShaderProgram : NonCopyable
{
public:
	ShaderProgram() = delete;
	ShaderProgram(const std::filesystem::path& aVertexShaderPath, const std::filesystem::path& aFragmentShaderPath, const std::filesystem::path& aGeometryShaderPath = "");

	ShaderProgram(ShaderProgram&& aOther) noexcept;
	ShaderProgram& operator=(ShaderProgram&& aOther) noexcept;

	~ShaderProgram();

	void SetShader(const EShaderType aShaderType, const std::filesystem::path& aShaderPath);

	bool Activate();

	template<typename TProperty>
	void SetProperty(std::string_view aName, TProperty aProperty)
	{
		if (!Activate())
			return;

		std::string name{ aName };
		TProperty uniformProperty = aProperty;

		Closure propertySetter = [this, name, uniformProperty]()
		{
			SystemPtr<OpenGLFacade> openGL;

			const i32 location = openGL->GetUniformVariableLocation(*myProgramHandle, name.data());

			openGL->SetUniformVariable(location, uniformProperty);
		};

		propertySetter();

		myUniformSetters.Add(std::string(aName), std::move(propertySetter));
	}

private:
	void SetUniforms();
	void Create();
	void Destroy();
	void Recreate();
	bool CheckIfUpToDate();

	std::optional<u32> myProgramHandle;

	Container<Closure, Key<std::string>> myUniformSetters;

	ResourceProxy<Shader<EShaderType::Vertex>> myVertexShader;
	ResourceProxy<Shader<EShaderType::Fragment>> myFragmentShader;
	std::optional<ResourceProxy<Shader<EShaderType::Geometry>>> myGeometryShader;
};

