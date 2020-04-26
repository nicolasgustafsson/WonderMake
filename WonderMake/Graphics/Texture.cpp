#include "pch.h"
#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include <std\stb_image.h>
#include "OpenGLFacade.h"

Texture::Texture(const std::filesystem::path& aPath)
{
	myPath = aPath;
	i32 channelCount;
	u8* rawPixelData = stbi_load(aPath.string().c_str(), &myWidth, &myHeight, &channelCount, 0);

	SystemPtr<OpenGLFacade> openGL;

	myTextureHandle = openGL->GenerateTexture();

	openGL->BindTexture(GL_TEXTURE_2D, myTextureHandle);

	openGL->SetTexureParameter(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	openGL->SetTexureParameter(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	openGL->SetTexureParameter(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	openGL->SetTexureParameter(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	const GLenum format = channelCount == 3 ? GL_RGB : GL_RGBA;

	openGL->UploadTextureData(GL_TEXTURE_2D, 0, format, myWidth, myHeight, format, GL_UNSIGNED_BYTE, rawPixelData);
	openGL->GenerateMipMapForCurrentTexture(GL_TEXTURE_2D);
}

Texture::~Texture()
{
	SystemPtr<OpenGLFacade> openGL;
	openGL->DeleteTexture(myTextureHandle);
}

void Texture::Bind(const u32 aTextureSlot)
{
	SystemPtr<OpenGLFacade> openGL;

	openGL->SetActiveTextureSlot(aTextureSlot);

	openGL->BindTexture(GL_TEXTURE_2D, myTextureHandle);
}
