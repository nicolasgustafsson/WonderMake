#pragma once

#include "Graphics/SpriteRenderObject.h"

#include "wondermake-base/Component.h"

#include <optional>

struct SSpriteComponent 
	: public STraitedComponent<
		STrait::Set<
			STWonderMake>>
{
	std::optional<SpriteRenderObject> RenderObject;
	bool IsHidden = false;
};
