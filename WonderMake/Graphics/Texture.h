#pragma once

#include "Resources/Resource.h"
#include "Assets/Asset.h"

class Texture
	: public Resource
	, public Asset
	, private NonCopyable
{
public:
	Texture(const std::filesystem::path& aPath);
	~Texture();

	void Bind(const u32 aTextureSlot = 0);

	i32 GetWidth() const noexcept { return myWidth; }
	i32 GetHeight() const noexcept { return myHeight; }

private:
	std::filesystem::path myPath;
	u32 myTextureHandle;

	i32 myWidth;
	i32 myHeight;
};






















namespace Test2
{




































}





















