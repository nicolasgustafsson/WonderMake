#pragma once

#include "wondermake-ui/OpenGLFacade.h"
#include "wondermake-ui/ShaderProgram.h"
#include "wondermake-ui/ShaderResource.h"

#include "wondermake-io/FileResourceProxy.h"

#include "wondermake-base/System.h"

#include "wondermake-utility/Future.h"

class FileResourceSystem;

class ShaderResourceSystem
	: public System<
		Policy::Set<
			PAdd<FileResourceSystem, PWrite>,
			PAdd<OpenGLFacade, PWrite>>,
		STrait::Set<
			STGui,
			STSingleton,
			STWonderMake>>
{
public:
	[[nodiscard]] Future<FileResourceRef<ShaderResource<EShaderType::Vertex>>>		GetShaderVertex(const FilePath& aPath);
	[[nodiscard]] Future<FileResourceRef<ShaderResource<EShaderType::Fragment>>>	GetShaderFragment(const FilePath& aPath);
	[[nodiscard]] Future<FileResourceRef<ShaderResource<EShaderType::Geometry>>>	GetShaderGeometry(const FilePath& aPath);

	[[nodiscard]] std::shared_ptr<ShaderProgram> CreateProgram(
		FileResourceRef<ShaderResource<EShaderType::Vertex>>	aShaderVertex,
		FileResourceRef<ShaderResource<EShaderType::Fragment>>	aShaderFragment,
		FileResourcePtr<ShaderResource<EShaderType::Geometry>>	aShaderGeometry);
	[[nodiscard]] Future<std::shared_ptr<ShaderProgram>> CreateProgram(
		const FilePath& aShaderVertex,
		const FilePath& aShaderFragment);
	[[nodiscard]] Future<std::shared_ptr<ShaderProgram>> CreateProgram(
		const FilePath& aShaderVertex,
		const FilePath& aShaderFragment,
		const FilePath& aShaderGeometry);

	bool ActivateProgram(const std::shared_ptr<ShaderProgram>& aProgram);
	
	template<typename TProperty>
	void SetProgramProperty(const std::shared_ptr<ShaderProgram>& aProgram, const char* aName, TProperty aProperty)
	{
		if (!ActivateProgram(aProgram))
			return;

		auto& openGL = Get<OpenGLFacade>();

		const i32 location = openGL.GetUniformVariableLocation(aProgram->Handle(), aName);

		openGL.SetUniformVariable(location, aProperty);
	}	

private:
	[[nodiscard]] u32 CreateProgramHandle(
		const FileResourceRef<ShaderResource<EShaderType::Vertex>>&		aShaderVertex,
		const FileResourceRef<ShaderResource<EShaderType::Fragment>>&	aShaderFragment,
		const FileResourcePtr<ShaderResource<EShaderType::Geometry>>&	aShaderGeometry);

};
