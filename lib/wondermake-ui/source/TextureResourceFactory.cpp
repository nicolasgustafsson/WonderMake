#include "TextureResourceFactory.h"

#include "wondermake-ui/OpenGLFacade.h"

#include "wondermake-base/SystemGlobal.h"

#include <std\stb_image.h>

WM_REGISTER_SYSTEM(TextureResourceFactory);

Future<SharedReference<TextureResource>> TextureResourceFactory::CreateResourceStrategy(FileResourceId /*aId*/, FilePath aPath, MakeResourceOp aMakeOp)
{
	i32 channelCount;
	SVector2i size;
	std::string pathString = aPath.GetFirstFileFromAllPaths().string();

	u8* rawPixelData = stbi_load(pathString.c_str(), &size.X, &size.Y, &channelCount, 0);

	if (!rawPixelData)
	{
		WmLogError(TagWonderMake << TagWmResources << "Failed to load texture: " << aPath << '.');

		auto texture = aMakeOp(0, SVector2i::Zero());

		return MakeCompletedFuture<SharedReference<TextureResource>>(std::move(texture));
	}

	auto& openGL = Get<OpenGLFacade>();

	u32 textureHandle = openGL.GenerateTexture();

	openGL.BindTexture(GL_TEXTURE_2D, textureHandle);

	openGL.SetTexureParameter(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	openGL.SetTexureParameter(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	openGL.SetTexureParameter(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	openGL.SetTexureParameter(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	const GLenum format = channelCount == 3 ? GL_RGB : GL_RGBA;

	openGL.UploadTextureData(GL_TEXTURE_2D, 0, format, size.X, size.Y, format, GL_UNSIGNED_BYTE, rawPixelData);
	openGL.GenerateMipMapForCurrentTexture(GL_TEXTURE_2D);

	stbi_image_free(rawPixelData);

	auto texture = aMakeOp(textureHandle, size);

	return MakeCompletedFuture<SharedReference<TextureResource>>(std::move(texture));
}

void TextureResourceFactory::DestroyResource(TextureResource& aResource)
{
	if (aResource.IsValid())
		Get<OpenGLFacade>().DeleteTexture(aResource.Handle());
}
