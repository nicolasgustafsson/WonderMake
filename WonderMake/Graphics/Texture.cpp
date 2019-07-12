#include "stdafx.h"
#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

Texture::Texture(const std::filesystem::path& aPath)
{
	i32 channelCount;
	u8* rawPixelData = stbi_load(aPath.string().c_str(), &myWidth, &myHeight, &channelCount, 0);

	glGenTextures(1, &myTextureHandle);
	glBindTexture(GL_TEXTURE_2D, myTextureHandle);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	const GLenum format = channelCount == 3 ? GL_RGB : GL_RGBA;

	glTexImage2D(GL_TEXTURE_2D, 0, format, myWidth, myHeight, 0, format, GL_UNSIGNED_BYTE, rawPixelData);
	glGenerateMipmap(GL_TEXTURE_2D);
}

Texture::~Texture()
{
	glDeleteTextures(1, &myTextureHandle);
}

void Texture::Bind(const u32 aTextureSlot)
{
	glActiveTexture(GL_TEXTURE0 + aTextureSlot);

	glBindTexture(GL_TEXTURE_2D, myTextureHandle);
}
