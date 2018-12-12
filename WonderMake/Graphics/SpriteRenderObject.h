#pragma once
#include "Graphics/RenderObject.h"

class SpriteRenderObject : public RenderObject<EVertexAttribute::Position, EVertexAttribute::Scale, EVertexAttribute::Color>
{
public:
	SpriteRenderObject(const std::filesystem::path& aTexturePath);
};

