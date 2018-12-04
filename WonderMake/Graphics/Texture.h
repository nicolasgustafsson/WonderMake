#pragma once
class Texture : NonCopyable
{
public:
	Texture(const std::filesystem::path& Path);
	~Texture();

	void Bind();

private:
	u32 myTextureHandle;

	i32 myWidth;
	i32 myHeight;
};

