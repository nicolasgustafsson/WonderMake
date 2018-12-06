#pragma once
class Texture : NonCopyable
{
public:
	Texture(const std::filesystem::path& Path);
	~Texture();

	void Bind(const u32 TextureSlot = 0);

private:
	u32 myTextureHandle;

	i32 myWidth;
	i32 myHeight;
};

