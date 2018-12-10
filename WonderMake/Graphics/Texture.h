#pragma once
class Texture : NonCopyable
{
public:
	Texture(const std::filesystem::path& aPath);
	~Texture();

	void Bind(const u32 aTextureSlot = 0);

	i32 GetWidth() const { return myWidth; }
	i32 GetHeight() const { return myHeight; }

private:
	u32 myTextureHandle;

	i32 myWidth;
	i32 myHeight;
};

