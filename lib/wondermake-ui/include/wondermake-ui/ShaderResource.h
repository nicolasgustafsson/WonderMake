#pragma once

#include "wondermake-io/FileResource.h"

#include "wondermake-utility/Typedefs.h"

enum class EShaderType
{
	Vertex,
	Fragment,
	Geometry
};

template<EShaderType ShaderType>
class ShaderResource
	: public FileResource<>
{
public:
	static constexpr EShaderType ShaderType = ShaderType;

	inline ShaderResource(u32 aHandle)
		: myHandle(aHandle)
	{}

	[[nodiscard]] inline u32 Handle() const noexcept
	{
		return myHandle;
	}

private:
	u32 myHandle;

};

template<>
[[nodiscard]] inline std::string_view GetFileResourceTypeName<ShaderResource<EShaderType::Vertex>>() noexcept
{
	return "Vertex Shader";
}
template<>
[[nodiscard]] inline std::string_view GetFileResourceTypeName<ShaderResource<EShaderType::Fragment>>() noexcept
{
	return "Fragment Shader";
}
template<>
[[nodiscard]] inline std::string_view GetFileResourceTypeName<ShaderResource<EShaderType::Geometry>>() noexcept
{
	return "Geometry Shader";
}
