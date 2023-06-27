#include "wondermake-ui/TextureResourceSystem.h"

#include "wondermake-ui/OpenGLFacade.h"

#include "wondermake-io/FileResourceSystem.h"

#include "wondermake-base/SystemGlobal.h"

WM_REGISTER_SYSTEM(TextureResourceSystem);

Future<FileResourceRef<TextureResource>> TextureResourceSystem::GetTexture(const FilePath& aPath)
{
	return Get<FileResourceSystem>()
		.GetResource<TextureResource>(aPath);
}

void TextureResourceSystem::Bind(const FileResourceRef<TextureResource>& aTexture, u32 aTextureSlot)
{
	auto& openGL = Get<OpenGLFacade>();

	openGL.SetActiveTextureSlot(aTextureSlot);

	openGL.BindTexture(GL_TEXTURE_2D, aTexture->Handle());
}
