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
	ShaderProgram(const std::filesystem::path& VertexShaderPath, const std::filesystem::path& FragmentShaderPath, const std::filesystem::path& GeometryShaderPath = "");
	~ShaderProgram();

	void Activate();

	template<typename TProperty>
	void SetProperty(std::string_view Name, TProperty Property)
	{
		if (!myProgramHandle)
			return;

		const auto Location = glGetUniformLocation(*myProgramHandle, Name.data());

		if constexpr (std::is_same_v<TProperty, i32>)
			glUniform1i(Location, Property);
		else if constexpr (std::is_same_v<TProperty, f32>)
			glUniform1f(Location, Property);
		else if constexpr (std::is_same_v<TProperty, bool>)
			glUniform1i(Location, Property);
		else if constexpr (std::is_same_v<TProperty, f64>)
			glUniform1d(Location, Property);
		else if constexpr (std::is_same_v<TProperty, SVector2<f32>>)
			glUniform2f(Location, Property.X, Property.Y);
		else if constexpr (std::is_same_v<TProperty, SVector3<f32>>)
			glUniform3f(Location, Property.X, Property.Y, Property.Z);
		else if constexpr (std::is_same_v<TProperty, SVector4<f32>>)
			glUniform4f(Location, Property.X, Property.Y, Property.Z, Property.W);
		else if constexpr (std::is_same_v<TProperty, SColor>)
			glUniform4f(Location, Property.R, Property.G, Property.B, Property.A);
	}

private:
	std::optional<u32> myProgramHandle;

	Shader<EShaderType::Vertex> myVertexShader;
	Shader<EShaderType::Fragment> myFragmentShader;
	std::optional<Shader<EShaderType::Geometry>> myGeometryShader;
};

