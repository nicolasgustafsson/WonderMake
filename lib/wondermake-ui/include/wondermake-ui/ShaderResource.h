#pragma once

#include "wondermake-ui/ShaderParsedData.h"

#include "wondermake-io/FileResource.h"

#include "wondermake-utility/Typedefs.h"

enum class EShaderType
{
	Vertex,
	Fragment,
	Geometry
};

enum class EShaderStatus
{
	Ok,
	Err
};

template<EShaderType ShaderType>
class ShaderResource
	: public FileResource<>
{
public:
	static constexpr EShaderType ShaderType = ShaderType;

	inline ShaderResource(ShaderParsedData aParsedData, u32 aHandle, std::string aStatusText, EShaderStatus aStatus)
		: myParsedData(std::move(aParsedData))
		, myHandle(aHandle)
		, myStatusText(std::move(aStatusText))
		, myStatus(aStatus)
	{}

	[[nodiscard]] inline u32 Handle() const noexcept
	{
		return myHandle;
	}

	[[nodiscard]] inline const ShaderParsedData& ParsedData() const noexcept
	{
		return myParsedData;
	}
	[[nodiscard]] inline const std::string& StatusText() const noexcept
	{
		return myStatusText;
	}
	[[nodiscard]] inline EShaderStatus Status() const noexcept
	{
		return myStatus;
	}

private:
	ShaderParsedData	myParsedData;
	u32					myHandle;
	std::string			myStatusText;
	EShaderStatus		myStatus;

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
