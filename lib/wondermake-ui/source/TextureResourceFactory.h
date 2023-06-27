#pragma once

#include "wondermake-ui/TextureResource.h"

#include "wondermake-io/FileResourceFactory.h"

class OpenGLFacade;

class TextureResourceFactory
	: public FileResourceFactory<
		TextureResource,
		Policy::Set<
			PAdd<OpenGLFacade, PWrite>>,
		STrait::Set<
			STGui,
			STSingleton,
			STWonderMake>>
{
protected:
	Future<SharedReference<TextureResource>> CreateResourceStrategy(FileResourceId aId, FilePath aPath, MakeResourceOp aMakeOp) override;
	void DestroyResource(TextureResource& aResource) override;

};
