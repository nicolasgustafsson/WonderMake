#include "stdafx.h"
#include "SpriteRenderObject.h"

SpriteRenderObject::SpriteRenderObject(const std::filesystem::path& aTexturePath)
	:RenderObject(SRenderObjectInfo
		{	std::filesystem::current_path() / "Shaders/Vertex/SpriteVertex.vert"
		,	std::filesystem::current_path() / "Shaders/Geometry/SpriteGeometry.geom"
		,	std::filesystem::current_path() / "Shaders/Fragment/SpriteFragment.frag"
		,	aTexturePath
		,	1 })
{
	myShaderProgram.SetProperty("TextureSize", SVector2f{ static_cast<float>(myTextures[0]->GetWidth()), static_cast<float>(myTextures[0]->GetHeight())});
}
