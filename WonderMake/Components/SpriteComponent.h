#pragma once
#include <optional>
#include "Graphics/SpriteRenderObject.h"
#include "Component.h"
#include "Resources/AssetLink.h"

struct SSpriteComponent 
	: public SComponent
{
	std::optional<SpriteRenderObject> RenderObject;
	bool IsHidden = false;
	SVector2f Scale = SVector2f::One();
};
