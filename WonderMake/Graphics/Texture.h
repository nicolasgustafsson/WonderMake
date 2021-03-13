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

	Texture(Texture&& aOther) noexcept
	{
		*this = std::move(aOther);
	}

	Texture& operator=(Texture&& aOther) noexcept
	{
		myPath = std::move(aOther.myPath);
		myTextureHandle = aOther.myTextureHandle;
		myWidth = aOther.myWidth;
		myHeight = aOther.myHeight;
		aOther.myTextureHandle = std::nullopt;

		return *this;
	}

	void Bind(const u32 aTextureSlot = 0);

	i32 GetWidth() const noexcept { return myWidth; }
	i32 GetHeight() const noexcept { return myHeight; }

private:
	std::filesystem::path myPath;
	std::optional<u32> myTextureHandle;

	i32 myWidth;
	i32 myHeight;
};






















namespace Test2
{




































}





















