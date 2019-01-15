#pragma once
#include "Graphics/Shader.h"
#include "Utilities/Vector.h"
#include "Utilities/Vector.h"
#include "Utilities/Vector.h"
#include <optional>

class ShaderProgram : NonCopyable
{
public:
	ShaderProgram() = delete;
	ShaderProgram(const std::filesystem::path& aVertexShaderPath, const std::filesystem::path& aFragmentShaderPath, const std::filesystem::path& aGeometryShaderPath = "");
	~ShaderProgram();

	void Activate();

	template<typename TProperty>
	void SetProperty(std::string_view aName, TProperty aProperty)
	{
		Activate();
		const auto Location = glGetUniformLocation(myProgramHandle, aName.data());

		if constexpr (std::is_same_v<TProperty, i32>)
			glUniform1i(Location, aProperty);
		else if constexpr (std::is_same_v<TProperty, f32>)
			glUniform1f(Location, aProperty);
		else if constexpr (std::is_same_v<TProperty, bool>)
			glUniform1i(Location, aProperty);
		else if constexpr (std::is_same_v<TProperty, f64>)
			glUniform1d(Location, aProperty);
		else if constexpr (std::is_same_v<TProperty, SVector2f>)
			glUniform2f(Location, aProperty.X, aProperty.Y);
		else if constexpr (std::is_same_v<TProperty, SVector2i>)
			glUniform2i(Location, aProperty.X, aProperty.Y);
		else if constexpr (std::is_same_v<TProperty, SVector3f>)
			glUniform3f(Location, aProperty.X, aProperty.Y, aProperty.Z);
		else if constexpr (std::is_same_v<TProperty, SVector4f>)
			glUniform4f(Location, aProperty.X, aProperty.Y, aProperty.Z, aProperty.W);
		else if constexpr (std::is_same_v<TProperty, SColor>)
			glUniform4f(Location, aProperty.R, aProperty.G, aProperty.B, aProperty.A);
	}

private:
	u32 myProgramHandle = std::numeric_limits<u32>::max();

	Shader<EShaderType::Vertex>* myVertexShader = nullptr;
	Shader<EShaderType::Fragment>* myFragmentShader = nullptr;
	std::optional<Shader<EShaderType::Geometry>*> myGeometryShader;
};

