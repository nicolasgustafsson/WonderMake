#pragma once
#include <optional>
#include "Graphics/SpriteRenderObject.h"
#include "Component.h"
#include "Resources/AssetLink.h"

struct SSpriteComponent 
	: public SComponent
{
	std::optional<SpriteRenderObject> RenderObject;
	SAssetLink<Texture> TextureAsset;
	bool IsHidden = false;
};
