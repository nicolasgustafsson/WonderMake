#pragma once

#include "wondermake-io/FileResource.h"

#include "wondermake-utility/Typedefs.h"
#include "wondermake-utility/Vector.h"

class TextureResource
	: public FileResource<>
{
public:
	inline TextureResource(u32 aHandle, SVector2i aSize)
		: myHandle(aHandle)
		, mySize(aSize)
	{}

	[[nodiscard]] inline u32 Handle() const noexcept { return myHandle; }
	[[nodiscard]] inline bool IsValid() const noexcept { return myHandle != 0; }
	[[nodiscard]] inline SVector2i Size() const noexcept { return mySize; }

private:
	u32			myHandle = 0;
	SVector2i	mySize;
};
