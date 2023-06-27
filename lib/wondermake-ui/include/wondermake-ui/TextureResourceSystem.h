#pragma once

#include "wondermake-ui/TextureResource.h"

#include "wondermake-io/FileResourceProxy.h"

#include "wondermake-base/System.h"

#include "wondermake-utility/Future.h"

class OpenGLFacade;

class FileResourceSystem;

class TextureResourceSystem
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
	[[nodiscard]] Future<FileResourceRef<TextureResource>> GetTexture(const FilePath& aPath);

	void Bind(const FileResourceRef<TextureResource>& aTexture, u32 aTextureSlot = 0);

};
