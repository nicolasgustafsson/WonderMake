#pragma once

#include "wondermake-ui/ShaderResource.h"

#include "wondermake-io/FileResourceFactory.h"

class OpenGLFacade;

template<EShaderType ShaderType>
class ShaderResourceFactory
	: public FileResourceFactory<
		ShaderResource<ShaderType>,
		Policy::Set<
			PAdd<OpenGLFacade, PWrite>>,
		STrait::Set<
			STGui,
			STSingleton,
			STWonderMake>>
{
protected:
	using MakeResourceOp = FileResourceFactory<
		ShaderResource<ShaderType>,
		Policy::Set<
			PAdd<OpenGLFacade, PWrite>>,
		STrait::Set<
			STGui,
			STSingleton,
			STWonderMake>>::MakeResourceOp;

	Future<SharedReference<ShaderResource<ShaderType>>> CreateResourceStrategy(FileResourceId aId, FilePath aPath, MakeResourceOp aMakeOp) override;
	void DestroyResource(ShaderResource<ShaderType>& aResource) override;

};
