#pragma once
#include "Functionality.h"
#include "Components/SpriteComponent.h"
#include "Components/TransformComponent.h"

class SpriteRenderingFunctionality
	: public Functionality<
		Policy::Set<
			PAdd<STransformComponent, PRead>,
			PAdd<SSpriteComponent, PWrite>>>
{
public:
	void Tick();
	void SetTexture(const std::string_view aAssetLink);
	void SetScale(const SVector2f aScale);
	void SetRotation(const f32 aRotation);
	void SetOrigin(const SVector2f aOrigin);
	void SetColor(const SColor aColor);
	void Hide() noexcept;
	void Show() noexcept;
};
