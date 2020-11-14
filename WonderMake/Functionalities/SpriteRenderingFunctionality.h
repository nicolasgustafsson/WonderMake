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
	SpriteRenderingFunctionality(Object& aOwner, Dependencies&& aDependencies);

	void Tick();
	void SetTexture(const std::filesystem::path& aTexturePath);
	void SetScale(const SVector2f aScale);
	void SetRotation(const f32 aRotation);
	void SetOrigin(const SVector2f aOrigin);
	void Hide() noexcept;
	void Show() noexcept;
};
