#pragma once
#include "Functionality.h"

#include "Components/SpriteComponent.h"
#include "Functionalities/TransformFunctionality.h"

class SpriteRenderingFunctionality
	: public Functionality<
		Policy::Set<
			PAdd<AssetDatabase<Texture>, PWrite>,
			PAdd<TransformFunctionality2D, PRead>,
			PAdd<SSpriteComponent, PWrite>>,
		SystemTraits::Set<
			STGui>>
{
public:
	void Tick();
	void SetTexture(std::string_view aAssetLink);
	void SetScale(const SVector2f aScale);
	void SetRotation(const f32 aRotation);
	void SetOrigin(const SVector2f aOrigin);
	void SetColor(const SColor aColor);
	void Hide() noexcept;
	void Show() noexcept;
};
