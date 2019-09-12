#pragma once
#include "Functionality.h"
#include "Components/SpriteComponent.h"
#include "Components/TransformComponent.h"
#include "TimeKeeper.h"

class SpriteRenderingFunctionality
	: public Functionality<SpriteRenderingFunctionality, STransformComponent, SSpriteComponent>
{
public:
	SpriteRenderingFunctionality(Object& aOwner);

	void Tick();
	void SetTexture(const std::filesystem::path& aTexturePath);
	void SetScale(const SVector2f aScale);
	void SetRotation(const f32 aRotation);
	void SetOrigin(const SVector2f aOrigin);
	void Hide();
	void Show();

private:
	SystemPtr<TimeKeeper> myTimeKeeperPtr;
};

