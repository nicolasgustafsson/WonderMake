#pragma once
#include "Functionality.h"

#include "Components/SpriteComponent.h"
#include "Functionalities/TransformFunctionality.h"

class SpriteRenderingFunctionality
	: public Functionality<TransformFunctionality2D, SSpriteComponent>
{
public:
	void Tick();
	void SetTexture(const std::string_view aAssetLink);
	void SetTexturePath(const std::filesystem::path aAssetPath);
	void SetScale(const SVector2f aScale);
	void SetRotation(const f32 aRotation);
	void SetOrigin(const SVector2f aOrigin);
	void SetColor(const SColor aColor);
	void Hide() noexcept;
	void Show() noexcept;
};
