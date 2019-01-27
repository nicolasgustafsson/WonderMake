#pragma once
#include "Functionality.h"
#include "Components/SpriteComponent.h"
#include "Components/TransformComponent.h"

class SpriteRenderingFunctionality
	: public Functionality<SpriteRenderingFunctionality, STransformComponent, SSpriteComponent>
{
public:
	SpriteRenderingFunctionality(Object& aOwner);
	~SpriteRenderingFunctionality() = default;

	void Tick();
	void SetTexture(const std::filesystem::path& aTexturePath);
	void SetScale(const SVector2f aScale);
};

