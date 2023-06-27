#pragma once

#include "wondermake-ui/ShaderResource.h"

#include "wondermake-io/FileResourceProxy.h"

#include "wondermake-utility/Typedefs.h"

class ShaderProgram
{
public:
	inline ShaderProgram(
		u32														aProgramHandle,
		FileResourceRef<ShaderResource<EShaderType::Vertex>>	aShaderVertex,
		FileResourceRef<ShaderResource<EShaderType::Fragment>>	aShaderFragment,
		FileResourcePtr<ShaderResource<EShaderType::Geometry>>	aShaderGeometry) noexcept
		: myProgramHandle(aProgramHandle)
		, myShaderVertex(std::move(aShaderVertex))
		, myShaderFragment(std::move(aShaderFragment))
		, myShaderGeometry(std::move(aShaderGeometry))
	{
		myGenerationVertex			= myShaderVertex->Generation();
		myGenerationFragment		= myShaderFragment->Generation();
		if (myShaderGeometry)
			myGenerationGeometry	= myShaderGeometry->Generation();
	}

	[[nodiscard]] inline u32 Handle() const noexcept
	{
		return myProgramHandle;
	}
	[[nodiscard]] inline void UpdateHandle(u32 aHandle) noexcept
	{
		myProgramHandle				= aHandle;
		myGenerationVertex			= myShaderVertex->Generation();
		myGenerationFragment		= myShaderFragment->Generation();
		if (myShaderGeometry)
			myGenerationGeometry	= myShaderGeometry->Generation();
	}
	[[nodiscard]] inline const FileResourceRef<ShaderResource<EShaderType::Vertex>>& ShaderVertex() const noexcept
	{
		return myShaderVertex;
	}
	[[nodiscard]] inline const FileResourceRef<ShaderResource<EShaderType::Fragment>>& ShaderFragment() const noexcept
	{
		return myShaderFragment;
	}
	[[nodiscard]] inline const FileResourcePtr<ShaderResource<EShaderType::Geometry>>& ShaderGeometry() const noexcept
	{
		return myShaderGeometry;
	}

	[[nodiscard]] inline bool OutOfDate() const noexcept
	{
		return myShaderVertex->Generation() != myGenerationVertex
			|| myShaderFragment->Generation() != myGenerationFragment
			|| (myShaderGeometry && myShaderGeometry->Generation() != myGenerationGeometry);
	}

private:
	u32														myProgramHandle;
	FileResourceRef<ShaderResource<EShaderType::Vertex>>	myShaderVertex;
	FileResourceRef<ShaderResource<EShaderType::Fragment>>	myShaderFragment;
	FileResourcePtr<ShaderResource<EShaderType::Geometry>>	myShaderGeometry;

	u32 myGenerationVertex		= 0;
	u32 myGenerationFragment	= 0;
	u32 myGenerationGeometry	= 0;

};
