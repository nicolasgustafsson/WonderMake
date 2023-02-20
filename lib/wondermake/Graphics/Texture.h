#pragma once

#include "Resources/Resource.h"

#include "wondermake-utility/RestrictTypes.h"

class Texture
	: public Resource
	, private NonCopyable
{
public:
	Texture(const std::filesystem::path& aPath);
	~Texture();

	void Bind(const u32 aTextureSlot = 0);

	inline [[nodiscard]] u32 GetTextureHandle() const noexcept { return myTextureHandle; }
	inline bool IsValid() const noexcept { return myTextureHandle != 0; }

	inline i32 GetWidth() const noexcept { return myWidth; }
	inline i32 GetHeight() const noexcept { return myHeight; }

private:
	std::filesystem::path myPath;
	u32 myTextureHandle = 0;

	i32 myWidth = 0;
	i32 myHeight = 0;
};
