#include "stdafx.h"
#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

Texture::Texture(const std::filesystem::path& Path)
{
	i32 ChannelCount;
	u8* RawPixelData = stbi_load(Path.string().c_str(), &myWidth, &myHeight, &ChannelCount, 0);

	glGenTextures(1, &myTextureHandle);
	glBindTexture(GL_TEXTURE_2D, myTextureHandle);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	const float BorderColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, BorderColor);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, myWidth, myHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, RawPixelData);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(RawPixelData);
}

Texture::~Texture()
{
	glDeleteTextures(1, &myTextureHandle);
}

void Texture::Bind(const u32 TextureSlot)
{
	glActiveTexture(GL_TEXTURE0 + TextureSlot);

	glBindTexture(GL_TEXTURE_2D, myTextureHandle);
}
