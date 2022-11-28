#pragma once

#include "Graphics/SpriteRenderObject.h"
#include "Resources/AssetLink.h"

#include "wondermake-base/Component.h"

#include <optional>

struct SSpriteComponent 
	: public SComponent
{
	std::optional<SpriteRenderObject> RenderObject;
	bool IsHidden = false;
};
