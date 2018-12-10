#pragma once
#include "Graphics/RenderObject.h"

class SpriteRenderObject : public RenderObject<EVertexAttribute::Position>
{
public:
	SpriteRenderObject(const std::filesystem::path& aTexturePath);
};

