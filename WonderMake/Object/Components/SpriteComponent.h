#pragma once
#include <optional>
#include <Graphics/SpriteRenderObject.h>
#include "../Component.h"

struct SSpriteComponent : 
	public SComponent
{
	std::optional<SpriteRenderObject> RenderObject;
};