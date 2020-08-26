#pragma once
#include "Functionality.h"
#include "Components/SpriteComponent.h"
#include "Components/TransformComponent.h"
#include "Utilities/TimeKeeper.h"

class SpriteRenderingFunctionality
	: public Functionality<
		SpriteRenderingFunctionality,
		Policy::Set<
			Policy::Add<STransformComponent, Policy::EPermission::Read>,
			Policy::Add<SSpriteComponent, Policy::EPermission::Write>>>
{
public:
	SpriteRenderingFunctionality(Object& aOwner);

	void Tick();
	void SetTexture(const std::filesystem::path& aTexturePath);
	void SetScale(const SVector2f aScale);
	void SetRotation(const f32 aRotation);
	void SetOrigin(const SVector2f aOrigin);
	void Hide() noexcept;
	void Show() noexcept;

private:
	SystemPtr<TimeKeeper> myTimeKeeperPtr;
};

REGISTER_FUNCTIONALITY(SpriteRenderingFunctionality);
