#pragma once

#include "wondermake-ui/FontResource.h"

#include "wondermake-io/FileResourceFactory.h"

class TextureResourceSystem;

class FontResourceFactory
	: public FileResourceFactory<
		FontResource,
		Policy::Set<
			PAdd<TextureResourceSystem, PWrite>>,
		STrait::Set<
			STGui,
			STSingleton,
			STWonderMake>>
{
protected:
	Future<SharedReference<FontResource>> CreateResourceStrategy(FileResourceId aId, FilePath aPath, MakeResourceOp aMakeOp) override;

};
