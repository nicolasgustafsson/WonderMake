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
		Activate();
		const auto Location = glGetUniformLocation(myProgramHandle, Name.data());
		auto err2 = glGetError();

		if constexpr (std::is_same_v<TProperty, i32>)
			glUniform1i(Location, Property);
		else if constexpr (std::is_same_v<TProperty, f32>)
			glUniform1f(Location, Property);
		else if constexpr (std::is_same_v<TProperty, bool>)
			glUniform1i(Location, Property);
		else if constexpr (std::is_same_v<TProperty, f64>)
			glUniform1d(Location, Property);
		else if constexpr (std::is_same_v<TProperty, SVector2f>)
			glUniform2f(Location, Property.X, Property.Y);
		else if constexpr (std::is_same_v<TProperty, SVector2i>)
			glUniform2i(Location, Property.X, Property.Y);
		else if constexpr (std::is_same_v<TProperty, SVector3f>)
			glUniform3f(Location, Property.X, Property.Y, Property.Z);
		else if constexpr (std::is_same_v<TProperty, SVector4f>)
			glUniform4f(Location, Property.X, Property.Y, Property.Z, Property.W);
		else if constexpr (std::is_same_v<TProperty, SColor>)
			glUniform4f(Location, Property.R, Property.G, Property.B, Property.A);
	}

private:
	u32 myProgramHandle = std::numeric_limits<u32>::max();

	Shader<EShaderType::Vertex>* myVertexShader = nullptr;
	Shader<EShaderType::Fragment>* myFragmentShader = nullptr;
	std::optional<Shader<EShaderType::Geometry>*> myGeometryShader;
};

